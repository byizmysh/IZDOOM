#version 450
layout(location=0) in vec2 uv;
layout(location=0) out vec4 color;
layout(set=0,binding=1,std430) readonly buffer VmMemory { uint words[]; } vm;
uint byteAt(uint a){return (vm.words[a/4u]>>((a&3u)*8u))&255u;}
void main(){
 vec2 p=(gl_FragCoord.xy-vec2(240,0))/vec2(1440,1080);
 if(p.x<0||p.x>=1||p.y<0||p.y>=1){color=vec4(0,0,0,1);return;}
 uint frame=vm.words[188u/4u],fault=vm.words[8u/4u];
 if(frame==0u){
  uint cycle=vm.words[12u/4u];float progress=min(1.0,float(cycle)/150000000.0);
  bool bar=p.y>.47&&p.y<.53&&p.x>.1&&p.x<.1+.8*progress;
  color=fault!=0u?vec4(.45,.025,.025,1):bar?vec4(.85,.28,.1,1):vec4(.018,.025,.035,1);return;
 }
 uint fb=vm.words[148u/4u],palette=vm.words[152u/4u];
 uint index=byteAt(fb+min(uint(p.y*200.0),199u)*320u+min(uint(p.x*320.0),319u));
 uint a=palette+index*3u;color=vec4(vec3(byteAt(a),byteAt(a+1u),byteAt(a+2u))/255.0,1);
 if(fault!=0u&&p.y<.01)color=vec4(1,0,0,1);
}
