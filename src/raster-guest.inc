#include "raster-abi.h"
static volatile uint32_t *const raster_state=(volatile uint32_t*)RASTER_STATE;
static volatile uint32_t *const raster_queue=(volatile uint32_t*)RASTER_QUEUE;
static void raster_flush(void){
 if(!raster_state[0])return;
 raster_state[2]++;
 register uint32_t operation __asm__("a7")=6;
 __asm__ volatile("ecall"::"r"(operation):"memory");
 raster_state[0]=0;
}
static void raster_append(uint32_t op,uint32_t dest,uint32_t source,uint32_t cmap,uint32_t count,uint32_t a,uint32_t b,uint32_t c,uint32_t d){
 if(raster_state[0]>=RASTER_CAPACITY)raster_flush();
 volatile uint32_t*q=raster_queue+raster_state[0]*RASTER_WORDS;
 q[0]=op;q[1]=dest;q[2]=source;q[3]=cmap;q[4]=count;q[5]=a;q[6]=b;q[7]=c;q[8]=d;q[9]=0;
 raster_state[0]++;
}
static void raster_column(void){
 int count=dc_yh-dc_yl;if(count<0)return;
 if((unsigned)dc_x>=SCREENWIDTH||dc_yl<0||dc_yh>=SCREENHEIGHT)I_Error("raster column bounds");
 uint32_t frac=(uint32_t)dc_texturemid+(uint32_t)(dc_yl-centery)*(uint32_t)dc_iscale;
 raster_append(1,(uint32_t)(uintptr_t)(ylookup[dc_yl]+columnofs[dc_x]),(uint32_t)(uintptr_t)dc_source,(uint32_t)(uintptr_t)dc_colormap,(uint32_t)count+1,frac,(uint32_t)dc_iscale,0,0);
}
static void raster_span(void){
 if(ds_x2<ds_x1||ds_x1<0||ds_x2>=SCREENWIDTH||(unsigned)ds_y>=SCREENHEIGHT)I_Error("raster span bounds");
 raster_append(2,(uint32_t)(uintptr_t)(ylookup[ds_y]+columnofs[ds_x1]),(uint32_t)(uintptr_t)ds_source,(uint32_t)(uintptr_t)ds_colormap,(uint32_t)(ds_x2-ds_x1+1),(uint32_t)ds_xfrac,(uint32_t)ds_yfrac,(uint32_t)ds_xstep,(uint32_t)ds_ystep);
}

static void raster_melt(void*dst,void*start,void*end,int*positions,int width,int height){
 raster_append(3,(uint32_t)(uintptr_t)dst,(uint32_t)(uintptr_t)start,
  (uint32_t)(uintptr_t)end,(uint32_t)(width*height),
  (uint32_t)(uintptr_t)positions,(uint32_t)width,(uint32_t)height,0);
 raster_flush();
}
