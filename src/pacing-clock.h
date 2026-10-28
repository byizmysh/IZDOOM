/* C/GLSL shared integer scheduler. Uses app-owned VM words0x180..0x1bf.
   The shell counter is read-only; no CPU caller or native timer is created. */
#define PCLOCK_MAGIC 0x180u
#define PCLOCK_READY 0x184u
#define PCLOCK_STARTED 0x188u
#define PCLOCK_LAST_LO 0x18cu
#define PCLOCK_LAST_HI 0x190u
#define PCLOCK_PHASE 0x194u
#define PCLOCK_TARGET 0x198u
#define PCLOCK_CONSUMED 0x19cu
#define PCLOCK_PREVIOUS_INPUT 0x1a0u
#define PCLOCK_PRESSED 0x1a4u
#define PCLOCK_DROPPED 0x1a8u
#define PCLOCK_ERROR 0x1acu
#define PCLOCK_PAUSES 0x1b0u
#define PCLOCK_FREQUENCY 31250000u
#define PCLOCK_MAX_GAP 7812500u
#define PCLOCK_BACKLOG 4u
void pacingBegin(uint lo,uint hi,uint frequency,uint buttons){
 if(MEM(PCLOCK_MAGIC)!=0x50434c31u){for(uint i=PCLOCK_MAGIC;i<0x1c0u;i+=4u)MEM(i)=0u;MEM(PCLOCK_MAGIC)=0x50434c31u;MEM(PCLOCK_PREVIOUS_INPUT)=buttons;}
 uint previous=MEM(PCLOCK_PREVIOUS_INPUT);MEM(PCLOCK_PREVIOUS_INPUT)=buttons;MEM(168u)=buttons;
 if(MEM(PCLOCK_READY)==0u){MEM(PCLOCK_PRESSED)=0u;MEM(PCLOCK_STARTED)=0u;return;}
 MEM(PCLOCK_PRESSED)|=buttons&~previous;
 if(frequency!=PCLOCK_FREQUENCY){MEM(PCLOCK_ERROR)=1u;MEM(PCLOCK_PRESSED)=0u;MEM(PCLOCK_STARTED)=0u;MEM(PCLOCK_TARGET)=MEM(PCLOCK_CONSUMED);return;}
 MEM(PCLOCK_ERROR)=0u;
 if(MEM(PCLOCK_STARTED)==0u){MEM(PCLOCK_STARTED)=1u;MEM(PCLOCK_LAST_LO)=lo;MEM(PCLOCK_LAST_HI)=hi;MEM(PCLOCK_PHASE)=0u;MEM(PCLOCK_TARGET)=MEM(PCLOCK_CONSUMED)+1u;}
 else{
  uint before=MEM(PCLOCK_LAST_LO),delta=lo-before,high=hi-MEM(PCLOCK_LAST_HI)-(lo<before?1u:0u);MEM(PCLOCK_LAST_LO)=lo;MEM(PCLOCK_LAST_HI)=hi;
  if(high!=0u||delta>PCLOCK_MAX_GAP){MEM(PCLOCK_PRESSED)=0u;MEM(PCLOCK_PHASE)=0u;MEM(PCLOCK_TARGET)=MEM(PCLOCK_CONSUMED);MEM(PCLOCK_PAUSES)++;}
  else{
   // delta<=frequency/4: this multiply/add is bounded below2^32.
   uint phase=MEM(PCLOCK_PHASE)+delta*35u,due=phase/PCLOCK_FREQUENCY;MEM(PCLOCK_PHASE)=phase%PCLOCK_FREQUENCY;
   uint backlog=MEM(PCLOCK_TARGET)-MEM(PCLOCK_CONSUMED),space=backlog<PCLOCK_BACKLOG?PCLOCK_BACKLOG-backlog:0u,accepted=due<space?due:space;
   MEM(PCLOCK_TARGET)+=accepted;MEM(PCLOCK_DROPPED)+=due-accepted;
  }
 }
 uint state=MEM(160u);if((state==1u||state==4u)&&MEM(PCLOCK_TARGET)!=MEM(PCLOCK_CONSUMED))MEM(160u)=0u;
}
uint pacingTakeTick(){
 MEM(PCLOCK_READY)=1u;
 if(MEM(PCLOCK_ERROR)!=0u||MEM(PCLOCK_STARTED)==0u||MEM(PCLOCK_TARGET)==MEM(PCLOCK_CONSUMED))return 0u;
 MEM(PCLOCK_CONSUMED)++;return MEM(PCLOCK_CONSUMED);
}
uint pacingInput(){uint buttons=MEM(168u)|MEM(PCLOCK_PRESSED);MEM(PCLOCK_PRESSED)=0u;return buttons;}
void pacingAfterFrame(){if(MEM(160u)==1u&&MEM(PCLOCK_TARGET)!=MEM(PCLOCK_CONSUMED)&&MEM(PCLOCK_ERROR)==0u)MEM(160u)=0u;}
