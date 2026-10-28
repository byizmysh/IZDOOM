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
