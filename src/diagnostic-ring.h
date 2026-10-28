/* App-owned record of the preceding actually drawn publication. */
#define DIAG_MAGIC 0x1ccu
#define DIAG_COUNT 0x1d0u
#define DIAG_LAST 0x1d4u
#define DIAG_RING 0x1740000u
#define DIAG_RECORDS 4096u
#define DIAG_STRIDE 96u
void diagnosticRingBegin(uint lo,uint hi){
 if(MEM(DIAG_MAGIC)!=0x44494131u){MEM(DIAG_MAGIC)=0x44494131u;MEM(DIAG_COUNT)=0u;MEM(DIAG_LAST)=0u;}
 uint frame=MEM(188u);
 if(frame==0u||frame==MEM(DIAG_LAST))return;
 uint count=MEM(DIAG_COUNT),q=DIAG_RING+(count%DIAG_RECORDS)*DIAG_STRIDE;
 MEM(q)=frame;MEM(q+4u)=MEM(0x1c0u);MEM(q+8u)=lo;MEM(q+12u)=hi;
 for(uint i=0u;i<20u;i++)MEM(q+16u+i*4u)=MEM(0x680u+i*4u);
 MEM(DIAG_LAST)=frame;MEM(DIAG_COUNT)=count+1u;
}
