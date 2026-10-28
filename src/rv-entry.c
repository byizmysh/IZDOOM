#include "doom_lite.h"
/* All pointers are offsets in the VM's private 32 MiB memory. */
#define WAD_ADDRESS 0x00100000u
#define HEAP_ADDRESS 0x00600000u
#define HEAP_SIZE DOOM_LITE_HEAP_BYTES
static uint32_t service(uint32_t op,uint32_t a,uint32_t b,uint32_t d){
 register uint32_t r0 __asm__("a0")=a,r1 __asm__("a1")=b,r2 __asm__("a2")=d,r7 __asm__("a7")=op;
 __asm__ volatile("ecall":"+r"(r0):"r"(r1),"r"(r2),"r"(r7):"memory");return r0;
}
static void panic(int code){service(3,(uint32_t)code,0,0);for(;;){}}
static void print(const char*s){service(2,(uint32_t)(uintptr_t)s,0,0);}
__attribute__((noreturn,section(".text.entry"))) void _start(void){
 uint32_t wad_bytes=service(4,0,0,0);
 DoomLiteConfig config={(const uint8_t*)WAD_ADDRESS,wad_bytes,(void*)HEAP_ADDRESS,HEAP_SIZE,print,panic};
 int result=doom_lite_boot(&config);if(result)panic(result);
 for(;;){uint32_t logicalTick=service(8,0,0,0);if(!logicalTick){service(9,0,0,0);continue;}const DoomLiteFrame*f=doom_lite_step_timed(service(5,0,0,0),logicalTick);service(1,(uint32_t)(uintptr_t)f->indexed,(uint32_t)(uintptr_t)f->palette,f->number);}
}
