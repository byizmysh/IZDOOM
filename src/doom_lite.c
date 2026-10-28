#include "doom_lite.h"
#include "replay-input.h"
static void raster_flush(void),raster_column(void),raster_span(void);
static void whole_render(void);
static void raster_melt(void*,void*,void*,int*,int,int);
#include "bulk32.h"
#define DOOM_IMPLEMENTATION
#include "../engine/PureDOOM-izdoom.h"
#include "raster-guest.inc"
#include "whole-guest.inc"
static DoomLiteConfig cfg;
static uint32_t tick_ms,previous_buttons;
static int initialized;
static const char *last_error="not initialized";
static DoomLiteFrame output;
typedef struct Block { size_t bytes; struct Block *next; int free; size_t padding; } Block;
static Block *blocks;
static size_t allocated,peak;
static void bytes_copy(void *dst,const void *src,size_t n){dl_bulk_copy(dst,src,n);}
static void bytes_zero(void *dst,size_t n){dl_bulk_fill(dst,0,n);}
static int same(const char*a,const char*b){while(*a&&*a==*b){a++;b++;}return *a==*b;}
static void die(int code){last_error=error_buf[0]?error_buf:"Doom engine stopped";cfg.panic(code);__builtin_trap();}
static void *allocate(int bytes){
 if(bytes<0)die(-20);size_t n=((size_t)bytes+15u)&~(size_t)15u;if(!n)n=16;
 for(Block*b=blocks;b;b=b->next)if(b->free&&b->bytes>=n){if(b->bytes>=n+sizeof(Block)+16){Block*tail=(Block*)((uint8_t*)(b+1)+n);tail->bytes=b->bytes-n-sizeof(Block);tail->next=b->next;tail->free=1;b->next=tail;b->bytes=n;}b->free=0;allocated+=b->bytes;if(allocated>peak)peak=allocated;return b+1;}
 last_error="bounded Doom heap exhausted";die(-21);return 0;
}
static void release(void *pointer){
 if(!pointer)return;Block*b=(Block*)pointer-1;if((uint8_t*)b<(uint8_t*)cfg.heap||(uint8_t*)b>=(uint8_t*)cfg.heap+cfg.heap_bytes||b->free)die(-22);allocated-=b->bytes;b->free=1;
 for(Block*x=blocks;x&&x->next;)if(x->free&&x->next->free){x->bytes+=sizeof(Block)+x->next->bytes;x->next=x->next->next;}else x=x->next;
}
typedef struct { size_t offset; int active; } ReadFile;
static ReadFile files[8];
static void *file_open(const char *name,const char *mode){
 if(!name||!mode||mode[0]!='r')return 0;const char*base=name;for(const char*s=name;*s;s++)if(*s=='/'||*s=='\\')base=s+1;if(!same(base,"doom1.wad"))return 0;
 for(unsigned i=0;i<8;i++)if(!files[i].active){files[i].active=1;files[i].offset=0;return &files[i];}return 0;
}
static void file_close(void *h){if(h)((ReadFile*)h)->active=0;}
static int file_read(void *h,void *dest,int n){if(!h||n<0)return -1;ReadFile*f=h;if(!f->active||f->offset>cfg.wad_bytes)return -1;size_t count=(size_t)n;if(count>cfg.wad_bytes-f->offset)count=cfg.wad_bytes-f->offset;bytes_copy(dest,cfg.wad+f->offset,count);f->offset+=count;return (int)count;}
static int file_write(void*h,const void*src,int n){(void)h;(void)src;(void)n;return -1;}
static int file_seek(void*h,int offset,doom_seek_t whence){if(!h)return -1;ReadFile*f=h;int64_t at=(whence==DOOM_SEEK_SET?0:whence==DOOM_SEEK_CUR?(int64_t)f->offset:whence==DOOM_SEEK_END?(int64_t)cfg.wad_bytes:-1);if(at<0||at+offset<0||(uint64_t)(at+offset)>cfg.wad_bytes)return -1;f->offset=(size_t)(at+offset);return 0;}
static int file_tell(void*h){return h?(int)((ReadFile*)h)->offset:-1;}
static int file_eof(void*h){return !h||((ReadFile*)h)->offset>=cfg.wad_bytes;}
static void clock_read(int *sec,int *usec){*sec=1+(int)(tick_ms/1000u);*usec=(int)(tick_ms%1000u)*1000;}
static char *environment(const char *name){return same(name,"HOME")?".":0;}
static void print_line(const char*text){if(cfg.print)cfg.print(text);}
int doom_lite_boot(const DoomLiteConfig *config){
 if(initialized)return -2;if(!config||!config->wad||config->wad_bytes<12||!config->heap||config->heap_bytes<DOOM_LITE_HEAP_BYTES||((uintptr_t)config->heap&15u)||!config->panic)return -1;
 if(config->wad[0]!='I'||config->wad[1]!='W'||config->wad[2]!='A'||config->wad[3]!='D')return -3;
 cfg=*config;
#ifndef DOOM_LITE_ZEROED_HEAP
 bytes_zero(cfg.heap,cfg.heap_bytes);
#endif
 blocks=cfg.heap;blocks->bytes=cfg.heap_bytes-sizeof(Block);blocks->next=0;blocks->free=1;allocated=peak=0;
 doom_set_print(print_line);doom_set_malloc(allocate,release);doom_set_file_io(file_open,file_close,file_read,file_write,file_seek,file_tell,file_eof);doom_set_gettime(clock_read);doom_set_exit(die);doom_set_getenv(environment);
 doom_set_default_int("sfx_volume",0);doom_set_default_int("music_volume",0);doom_set_default_int("screenblocks",11);doom_set_default_int("mouse_move",0);doom_set_default_int("crosshair",0);
 static char *args[]={"doom-lite","-iwad","doom1.wad","-warp","1","1","-skill","1"};
 mb_used=6;
 doom_init(8,args,DOOM_FLAG_HIDE_MOUSE_OPTIONS|DOOM_FLAG_HIDE_SOUND_OPTIONS|DOOM_FLAG_HIDE_MUSIC_OPTIONS);
 output.width=320;output.height=200;output.number=0;output.palette=screen_palette;initialized=1;last_error=0;return 0;
}
const DoomLiteFrame *doom_lite_step_timed(uint32_t buttons,uint32_t logicalTick){
 if(!initialized)return 0;if(*(volatile uint32_t*)0x1d8==0x52504c31u)buttons=replayMovie(output.number);
 static const doom_key_t keys[]={DOOM_KEY_UP_ARROW,DOOM_KEY_DOWN_ARROW,DOOM_KEY_LEFT_ARROW,DOOM_KEY_RIGHT_ARROW,DOOM_KEY_CTRL,DOOM_KEY_SPACE,DOOM_KEY_ENTER,DOOM_KEY_ESCAPE,DOOM_KEY_ALT};
 for(unsigned i=0;i<sizeof(keys)/sizeof(keys[0]);i++){uint32_t bit=1u<<i;if((buttons&bit)!=(previous_buttons&bit)){if(buttons&bit)doom_key_down(keys[i]);else doom_key_up(keys[i]);}}
 previous_buttons=buttons;tick_ms=(logicalTick/35u)*1000u+((logicalTick%35u)*1000u)/35u;doom_force_update();output.indexed=crosshair?doom_get_framebuffer(1):screens[0];output.number++;
 {volatile uint32_t*d=(volatile uint32_t*)0x600;player_t*p=&players[consoleplayer];mobj_t*m=p->mo;
 d[0]=output.number;d[1]=gametic;d[2]=m?m->x:0;d[3]=m?m->y:0;d[4]=m?m->z:0;d[5]=m?m->angle:0;
 d[6]=viewx;d[7]=viewy;d[8]=viewz;
 sector_t*sec=m&&m->subsector?m->subsector->sector:0;
 d[9]=(uint32_t)(uintptr_t)sec;d[10]=menuactive;d[11]=paused;d[12]=gamestate;d[13]=automapactive;
 d[14]=sec?sec->floorheight:0;d[15]=sec?sec->ceilingheight:0;d[16]=buttons;d[17]=leveltime;d[18]=gamemap;d[19]=gameepisode;}
return &output;
}
const char *doom_lite_error(void){return last_error;}
size_t doom_lite_heap_peak(void){return peak;}
/* Freestanding compiler-generated bulk operations, no OS or SDK imports. */
void *memcpy(void*d,const void*s,size_t n){bytes_copy(d,s,n);return d;}
void *memset(void*d,int v,size_t n){return dl_bulk_fill(d,v,n);}
void *memmove(void*d,const void*s,size_t n){uint8_t*a=d;const uint8_t*b=s;if(a<b)bytes_copy(d,s,n);else while(n){n--;a[n]=b[n];}return d;}
