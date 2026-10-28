// Minimal wall-mesh ray renderer. Source geometry/sectors remain read-only until this completes.
const uint WHOLE_STATE=0x400u,WHOLE_COUNTS=0x13f0000u,WHOLE_HITS=0x1400000u,WHOLE_MAX_HITS=128u,WHOLE_HIT_BYTES=28u;
shared uint wholeReady,wholeError,wholeFb,wholeLines,wholeNumLines,wholeWidth,wholeHeight,wholeHealth;
shared vec2 wholeCamera,wholeForward;
shared float wholeZ;
float wholeFixed(uint a){return float(int(MEM(a)))*(1.0/65536.0);}
float wholeCross(vec2 a,vec2 b){return a.x*b.y-a.y*b.x;}
bool wholeRange(uint a,uint n){return a<=LIMIT-n;}
vec2 wholeVertex(uint a){return vec2(wholeFixed(a),wholeFixed(a+4u));}
void wholePutHit(uint x,uint index,float depth,float top,float bottom,float lowerTop,float lowerBottom,uint color){
 uint q=WHOLE_HITS+(x*WHOLE_MAX_HITS+index)*WHOLE_HIT_BYTES;
 MEM(q)=floatBitsToUint(depth);MEM(q+4u)=floatBitsToUint(top);MEM(q+8u)=floatBitsToUint(bottom);
 MEM(q+12u)=floatBitsToUint(lowerTop);MEM(q+16u)=floatBitsToUint(lowerBottom);MEM(q+20u)=color;MEM(q+24u)=0u;
}
// Frozen-world live thinker traversal; no guest-side actor copy or new loads.
const uint ACTOR_STATE=0x1600000u,ACTOR_RECORDS=0x1601000u,ACTOR_COLUMN_IDS=0x1610000u;
const uint ACTOR_MAX=256u,ACTOR_COLUMN_MAX=64u,ACTOR_BYTES=32u;
shared uint actorCount;
void actorPrepare(){
 actorCount=0u;uint head=MEM(WHOLE_STATE+56u),callback=MEM(WHOLE_STATE+60u),player=MEM(WHOLE_STATE+64u),visited=0u;
 if(wholeError!=0u)return;
 if(!wholeRange(head,12u)){wholeError=16u;return;}
 uint node=MEM(head+4u);
 while(node!=head&&visited<4096u){
  if(node==0u||!wholeRange(node,12u)){wholeError=16u;return;}
  uint next=MEM(node+4u),function=MEM(node+8u);visited++;
  if(function==callback&&node!=player){
   if(!wholeRange(node,156u)){wholeError=16u;return;}
   uint flags=MEM(node+104u);int health=int(MEM(node+108u));
   // MF_NOSECTOR is invisible. MF_NOBLOCKMAP is not a visibility filter.
   bool corpse=(flags&0x400000u)!=0u&&health<=0;
   bool drawable=(flags&8u)==0u&&((flags&0x410005u)!=0u||corpse);
   if(drawable){
    vec2 offset=vec2(wholeFixed(node+12u),wholeFixed(node+16u))-wholeCamera;
    float depth=dot(offset,wholeForward);
    if(depth>0.1){
     float side=dot(offset,vec2(wholeForward.y,-wholeForward.x)),scale=160.0/depth;
     float radius=max(2.0,wholeFixed(node+64u)),height=max(8.0,wholeFixed(node+68u));
     uint kind=0u,color=176u;
     if(corpse){kind=4u;color=80u;height=8.0;}
     else if((flags&0x10000u)!=0u){kind=3u;color=160u;}
     else if((flags&1u)!=0u){kind=1u;color=112u;}
     else if((flags&0x400000u)==0u){kind=2u;color=64u;}
     float center=160.0+side*scale,left=center-radius*scale,right=center+radius*scale;
     float base=wholeFixed(node+20u),top=100.0-(base+height-wholeZ)*scale,bottom=100.0-(base-wholeZ)*scale;
     if(right>0.0&&left<320.0&&bottom>0.0&&top<200.0){
      if(actorCount>=ACTOR_MAX){wholeError=32u;return;}
      uint q=ACTOR_RECORDS+actorCount*ACTOR_BYTES;
      MEM(q)=floatBitsToUint(depth);MEM(q+4u)=floatBitsToUint(left);MEM(q+8u)=floatBitsToUint(right);
      MEM(q+12u)=floatBitsToUint(top);MEM(q+16u)=floatBitsToUint(bottom);MEM(q+20u)=color;MEM(q+24u)=kind;MEM(q+28u)=flags;
      actorCount++;
     }
    }
   }
  }
  node=next;
 }
 if(node!=head){wholeError=64u;return;}
 MEM(ACTOR_STATE)=actorCount;MEM(ACTOR_STATE+4u)=visited;
}
void actorColumn(uint x){
 uint count=0u;float sampleX=float(x)+0.5;
 for(uint i=0u;i<actorCount;i++){
  uint q=ACTOR_RECORDS+i*ACTOR_BYTES;float left=uintBitsToFloat(MEM(q+4u)),right=uintBitsToFloat(MEM(q+8u));
  if(sampleX>=left&&sampleX<right){
   if(count>=ACTOR_COLUMN_MAX){atomicOr(wholeError,128u);break;}
   MEM(ACTOR_COLUMN_IDS+(x*ACTOR_COLUMN_MAX+count)*4u)=i;count++;
  }
 }
 MEM(ACTOR_STATE+64u+x*4u)=count;
}
bool actorMask(uint kind,float u,float v,uint flags,uint x,uint y){
 if(u< -1.0||u>=1.0||v<0.0||v>=1.0)return false;
 if((flags&0x40000u)!=0u&&((x+y)&1u)==0u)return false; // Transparent checker holes for MF_SHADOW.
 if(kind==1u)return abs(u)<0.22||abs(v-0.5)<0.11; // Pickup cross.
 if(kind==2u)return abs(u)<0.75; // Shootable barrel/furniture.
 if(kind==3u){float yy=v*2.0-1.0;return u*u+yy*yy<0.75;} // Projectile disc.
 if(kind==4u)return u*u+(v*2.0-1.0)*(v*2.0-1.0)<0.8; // Short corpse pile.
 if(v<0.25){float yy=(v-0.125)/0.125;return u*u/(0.42*0.42)+yy*yy<1.0;}
 if(v<0.7)return abs(u)<0.55;
 return abs(u)>0.12&&abs(u)<0.48; // Two separate legs, transparent background.
}
void actorShade(uint x,uint y,inout float nearest,inout uint color){
 uint count=MEM(ACTOR_STATE+64u+x*4u);
 for(uint i=0u;i<count;i++){
  uint id=MEM(ACTOR_COLUMN_IDS+(x*ACTOR_COLUMN_MAX+i)*4u),q=ACTOR_RECORDS+id*ACTOR_BYTES;
  float depth=uintBitsToFloat(MEM(q));if(depth>=nearest)continue;
  float left=uintBitsToFloat(MEM(q+4u)),right=uintBitsToFloat(MEM(q+8u)),top=uintBitsToFloat(MEM(q+12u)),bottom=uintBitsToFloat(MEM(q+16u));
  float u=(float(x)+0.5-left)/(right-left)*2.0-1.0,v=(float(y)+0.5-top)/(bottom-top);
  if(actorMask(MEM(q+24u),u,v,MEM(q+28u),x,y)){nearest=depth;color=MEM(q+20u);}
 }
}

void wholePass(){
 uint lane=gl_LocalInvocationIndex;
 if(lane==0u){
  wholeReady=MEM(WHOLE_STATE);wholeError=0u;
  wholeFb=MEM(WHOLE_STATE+4u);wholeCamera=vec2(wholeFixed(WHOLE_STATE+8u),wholeFixed(WHOLE_STATE+12u));wholeZ=wholeFixed(WHOLE_STATE+16u);
  wholeForward=vec2(wholeFixed(WHOLE_STATE+20u),wholeFixed(WHOLE_STATE+24u));
  wholeNumLines=MEM(WHOLE_STATE+28u);wholeLines=MEM(WHOLE_STATE+32u);
  wholeWidth=MEM(WHOLE_STATE+36u);wholeHeight=MEM(WHOLE_STATE+40u);wholeHealth=MEM(WHOLE_STATE+44u);
  if(wholeReady!=0u&&(wholeWidth!=320u||wholeHeight!=200u||wholeNumLines>4096u||!wholeRange(wholeLines,wholeNumLines*64u)||!wholeRange(wholeFb,64000u)||(wholeFb&3u)!=0u))wholeError=1u;
 }
 barrier();
 if(wholeReady==0u)return;
 if(lane==0u)actorPrepare();memoryBarrierBuffer();barrier();
 if(wholeError==0u){
  for(uint x=lane;x<320u;x+=256u){
   float side=(float(x)+0.5-160.0)/160.0;
   // Doom viewx/viewy and positive angles use left-handed screen traversal.
   vec2 ray=wholeForward+vec2(wholeForward.y,-wholeForward.x)*side;
   uint count=0u;
   for(uint i=0u;i<wholeNumLines;i++){
    uint l=wholeLines+i*64u,p1=MEM(l),p2=MEM(l+4u),front=MEM(l+48u),back=MEM(l+52u);
    if(!wholeRange(p1,8u)||!wholeRange(p2,8u)){atomicOr(wholeError,2u);continue;}
    vec2 a=wholeVertex(p1),delta=wholeVertex(p2)-a,q=a-wholeCamera;
    float denominator=wholeCross(ray,delta);
    if(abs(denominator)<0.000001)continue;
    float depth=wholeCross(q,delta)/denominator,u=wholeCross(q,ray)/denominator;
    if(depth<=0.1||u<0.0||u>=1.0)continue;
    // Doom sidedef0/front is the right-hand side of the directed segment.
    if(wholeCross(delta,wholeCamera-a)>0.0){uint temporary=front;front=back;back=temporary;}
    if(front==0u)continue;
    if(!wholeRange(front,88u)||(back!=0u&&!wholeRange(back,88u))){atomicOr(wholeError,4u);continue;}
    float floorZ=wholeFixed(front),ceilingZ=wholeFixed(front+4u),scale=160.0/depth;
    float top=100.0-(ceilingZ-wholeZ)*scale,bottom=100.0-(floorZ-wholeZ)*scale;
    float lowerTop=201.0,lowerBottom=201.0;
    if(back!=0u){
     float backFloor=wholeFixed(back),backCeiling=wholeFixed(back+4u);
     bottom=ceilingZ>backCeiling?100.0-(backCeiling-wholeZ)*scale:top;
     if(backFloor>floorZ){lowerTop=100.0-(backFloor-wholeZ)*scale;lowerBottom=100.0-(floorZ-wholeZ)*scale;}
    }
    if(top==bottom&&lowerTop==lowerBottom)continue;
    if(count>=WHOLE_MAX_HITS){atomicOr(wholeError,8u);break;}
    uint light=MEM(front+12u)&65535u;
    uint color=96u+min(15u,(255u-min(255u,light))/16u)+(abs(delta.x)>abs(delta.y)?8u:0u);
    wholePutHit(x,count,depth,top,bottom,lowerTop,lowerBottom,color);count++;
   }
   MEM(WHOLE_COUNTS+x*4u)=count;
   actorColumn(x);
  }
 }
 memoryBarrierBuffer();barrier();
 if(wholeError==0u){
  // A lane owns a whole uint32. Indexed-byte writes cannot race.
  for(uint w=lane;w<16000u;w+=256u){
   uint packed=0u;
   for(uint j=0u;j<4u;j++){
    uint pixel=w*4u+j,x=pixel%320u,y=pixel/320u;float sampleY=float(y)+0.5,nearest=3.402823e38;
    uint color=y<100u?8u:64u,count=MEM(WHOLE_COUNTS+x*4u);
    for(uint i=0u;i<count;i++){
     uint q=WHOLE_HITS+(x*WHOLE_MAX_HITS+i)*WHOLE_HIT_BYTES;
     float depth=uintBitsToFloat(MEM(q)),top=uintBitsToFloat(MEM(q+4u)),bottom=uintBitsToFloat(MEM(q+8u));
     float lowerTop=uintBitsToFloat(MEM(q+12u)),lowerBottom=uintBitsToFloat(MEM(q+16u));
     if(depth<nearest&&((sampleY>=top&&sampleY<bottom)||(sampleY>=lowerTop&&sampleY<lowerBottom))){nearest=depth;color=MEM(q+20u);}
    }
    actorShade(x,y,nearest,color);
    if((x==160u&&y>=98u&&y<=102u)||(y==100u&&x>=158u&&x<=162u))color=4u;
    if(y>=196u&&x<min(100u,wholeHealth)*3u)color=112u;
    packed|=(color&255u)<<(j*8u);
   }
   MEM(wholeFb+w*4u)=packed;
  }
 }
 memoryBarrierBuffer();barrier();
 if(lane==0u){if(wholeError!=0u)MEM(8u)=10u;else{MEM(WHOLE_STATE)=0u;if(MEM(160u)==3u)MEM(160u)=0u;}}
}
