/* Shared C/GLSL counter. Begin runs exactly once before each outer draw sequence.
   The preceding submission drew the framebuffer whose publication is now read.
   A jump of many guest publications therefore counts as only one drawn view.
   This measures GPU draw cadence, not display scanout or photon latency. */
#define SL_OUTER_FRAMES 0x1c0u
#define SL_LAST_DRAWN_PUBLICATION 0x1c4u
#define SL_DISTINCT_DRAWN_VIEWS 0x1c8u
void presentationCountBegin(){
 uint publication=MEM(188u);
 MEM(SL_OUTER_FRAMES)++;
 if(publication!=0u&&publication!=MEM(SL_LAST_DRAWN_PUBLICATION)){
  MEM(SL_LAST_DRAWN_PUBLICATION)=publication;
  MEM(SL_DISTINCT_DRAWN_VIEWS)++;
 }
}
