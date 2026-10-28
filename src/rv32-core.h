/* RV32IM interpreter shared verbatim by C host verification and GLSL compute.
   Only guest memory and VM status can be written. No native host calls. */
#ifdef VM_GPU_SHARED_REGS
shared uint r[32];
#else
uint r[32];
#endif
uint pc,fault,cyclesLo,cyclesHi,stopped;
uint read8(uint a){if(a>=0x02000000u){fault=2u;return 0u;}return (MEM(a) >> ((a&3u)*8u))&255u;}
uint read16(uint a){return read8(a)|(read8(a+1u)<<8u);}
uint read32(uint a){if(a>0x01fffffcu){fault=2u;return 0u;}if((a&3u)==0u)return MEM(a);return read8(a)|(read8(a+1u)<<8u)|(read8(a+2u)<<16u)|(read8(a+3u)<<24u);}
void write8(uint a,uint value){if(a>=0x02000000u){fault=3u;return;}uint shift=(a&3u)*8u;MEM(a)=(MEM(a)&~(255u<<shift))|((value&255u)<<shift);}
void write16(uint a,uint value){write8(a,value);write8(a+1u,value>>8u);}
void write32(uint a,uint value){if(a>0x01fffffcu){fault=3u;return;}if((a&3u)==0u)MEM(a)=value;else{write8(a,value);write8(a+1u,value>>8u);write8(a+2u,value>>16u);write8(a+3u,value>>24u);}}
uint sx(uint value,uint bits){return U(I(value<<(32u-bits))>>I(32u-bits));}
uint mulhigh(uint a,uint b){uint a0=a&65535u,a1=a>>16u,b0=b&65535u,b1=b>>16u;uint low=a0*b0,t=a1*b0+(low>>16u),carry=t>>16u;t=(t&65535u)+a0*b1;return a1*b1+carry+(t>>16u);}
uint signedDivide(uint a,uint b){if(b==0u)return 0xffffffffu;if(a==0x80000000u&&b==0xffffffffu)return a;uint aa=I(a)<0?0u-a:a,bb=I(b)<0?0u-b:b;uint q=aa/bb;return (I(a)<0)!=(I(b)<0)?0u-q:q;}
uint signedRemain(uint a,uint b){if(b==0u)return a;if(a==0x80000000u&&b==0xffffffffu)return 0u;uint aa=I(a)<0?0u-a:a,bb=I(b)<0?0u-b:b;uint q=aa%bb;return I(a)<0?0u-q:q;}
#include "pacing-clock.h"
void vmService(){
 uint op=r[17];
 if(op==1u){MEM(148u)=r[10];MEM(152u)=r[11];MEM(156u)=r[12];MEM(160u)=1u;stopped=1u;}
 else if(op==2u){MEM(172u)=r[10];LOG_LINE(r[10]);}
 else if(op==3u){MEM(176u)=r[10];fault=5u;}
 else if(op==4u)r[10]=MEM(164u);
 else if(op==5u)r[10]=pacingInput();
 else if(op==6u){MEM(0x104u)=1u;MEM(160u)=2u;stopped=2u;}
 else if(op==7u){MEM(0x400u)=1u;MEM(160u)=3u;stopped=3u;}
 else if(op==8u)r[10]=pacingTakeTick();
 else if(op==9u){MEM(160u)=4u;stopped=4u;}
 else fault=6u;
}
void vmRun(uint budget){
 uint lastInstruction=MEM(180u);
 pc=MEM(4u);fault=MEM(8u);cyclesLo=MEM(12u);cyclesHi=MEM(144u);stopped=MEM(160u);
 for(uint j=0u;j<32u;j++)r[j]=MEM(16u+j*4u);
 for(uint step=0u;step<budget;step++){
  if(fault!=0u||stopped!=0u)break;
  if((pc&3u)!=0u){fault=1u;break;}
  uint ins=read32(pc),opcode=ins&127u,rd=(ins>>7u)&31u,fn=(ins>>12u)&7u,a=r[(ins>>15u)&31u],b=r[(ins>>20u)&31u],f7=ins>>25u;
  uint res=0u,imm=sx(ins>>20u,12u),nextpc=pc+4u;bool storeResult=false;
  lastInstruction=ins;
  if(opcode==0x37u){res=ins&0xfffff000u;storeResult=true;}
  else if(opcode==0x17u){res=pc+(ins&0xfffff000u);storeResult=true;}
  else if(opcode==0x6fu){uint v=((ins>>31u)<<20u)|(((ins>>12u)&255u)<<12u)|(((ins>>20u)&1u)<<11u)|(((ins>>21u)&1023u)<<1u);res=nextpc;nextpc=pc+sx(v,21u);storeResult=true;}
  else if(opcode==0x67u&&fn==0u){res=nextpc;nextpc=(a+imm)&~1u;storeResult=true;}
  else if(opcode==0x63u){uint v=((ins>>31u)<<12u)|(((ins>>7u)&1u)<<11u)|(((ins>>25u)&63u)<<5u)|(((ins>>8u)&15u)<<1u);bool take=false;
   if(fn==0u)take=a==b;else if(fn==1u)take=a!=b;else if(fn==4u)take=I(a)<I(b);else if(fn==5u)take=I(a)>=I(b);else if(fn==6u)take=a<b;else if(fn==7u)take=a>=b;else fault=4u;if(take)nextpc=pc+sx(v,13u);
  }
  else if(opcode==0x03u){uint address=a+imm;storeResult=true;if(fn==0u)res=sx(read8(address),8u);else if(fn==1u)res=sx(read16(address),16u);else if(fn==2u)res=read32(address);else if(fn==4u)res=read8(address);else if(fn==5u)res=read16(address);else fault=4u;}
  else if(opcode==0x23u){uint address=a+sx(((ins>>25u)<<5u)|((ins>>7u)&31u),12u);if(fn==0u)write8(address,b);else if(fn==1u)write16(address,b);else if(fn==2u)write32(address,b);else fault=4u;}
  else if(opcode==0x13u){storeResult=true;if(fn==0u)res=a+imm;else if(fn==2u)res=I(a)<I(imm)?1u:0u;else if(fn==3u)res=a<imm?1u:0u;else if(fn==4u)res=a^imm;else if(fn==6u)res=a|imm;else if(fn==7u)res=a&imm;else if(fn==1u&&f7==0u)res=a<<((ins>>20u)&31u);else if(fn==5u&&(f7==0u||f7==32u))res=f7==32u?U(I(a)>>I((ins>>20u)&31u)):a>>((ins>>20u)&31u);else fault=4u;}
  else if(opcode==0x33u){storeResult=true;
   if(f7==1u){if(fn==0u)res=a*b;else if(fn==1u)res=mulhigh(a,b)-(I(a)<0?b:0u)-(I(b)<0?a:0u);else if(fn==2u)res=mulhigh(a,b)-(I(a)<0?b:0u);else if(fn==3u)res=mulhigh(a,b);else if(fn==4u)res=signedDivide(a,b);else if(fn==5u)res=b==0u?0xffffffffu:a/b;else if(fn==6u)res=signedRemain(a,b);else res=b==0u?a:a%b;}
   else if(f7==0u||f7==32u){if(fn==0u)res=f7==32u?a-b:a+b;else if(fn==1u&&f7==0u)res=a<<(b&31u);else if(fn==2u&&f7==0u)res=I(a)<I(b)?1u:0u;else if(fn==3u&&f7==0u)res=a<b?1u:0u;else if(fn==4u&&f7==0u)res=a^b;else if(fn==5u)res=f7==32u?U(I(a)>>I(b&31u)):a>>(b&31u);else if(fn==6u&&f7==0u)res=a|b;else if(fn==7u&&f7==0u)res=a&b;else fault=4u;}
   else fault=4u;
  }
  else if(opcode==0x0fu){/* single sequential interpreter: FENCE is already satisfied */}
  else if(ins==0x00000073u)vmService();
  else fault=4u;
  if(storeResult&&rd!=0u)r[rd]=res;r[0]=0u;pc=nextpc;
  cyclesLo++;if(cyclesLo==0u)cyclesHi++;
 }
 MEM(180u)=lastInstruction;
 MEM(4u)=pc;MEM(8u)=fault;MEM(12u)=cyclesLo;MEM(144u)=cyclesHi;
 for(uint j=0u;j<32u;j++)MEM(16u+j*4u)=r[j];
}
