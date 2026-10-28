/* Forward-byte semantics, including overlapping dst>src; word fast path only
   on aligned accesses within the requested span. Compile -fno-strict-aliasing. */
#include <stdint.h>
#include <stddef.h>
static void *dl_bulk_copy(void *dst,const void *src,size_t n){
 uint8_t*d=dst;const uint8_t*s=src;uintptr_t da=(uintptr_t)d,sa=(uintptr_t)s;
 if(((da^sa)&3u)==0u&&(da<=sa||da-sa>=n)){
  while(n&&((uintptr_t)d&3u)){*d++=*s++;n--;}
  while(n>=4u){*(uint32_t*)d=*(const uint32_t*)s;d+=4;s+=4;n-=4;}
 }
 while(n){*d++=*s++;n--;}return dst;
}
static void *dl_bulk_fill(void *dst,int value,size_t n){
 uint8_t*d=dst;uint8_t v=(uint8_t)value;
 while(n&&((uintptr_t)d&3u)){*d++=v;n--;}
 uint32_t w=(uint32_t)v*0x01010101u;
 while(n>=4u){*(uint32_t*)d=w;d+=4;n-=4;}
 while(n){*d++=v;n--;}return dst;
}
