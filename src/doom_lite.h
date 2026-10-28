#ifndef IZ_DOOM_LITE_H
#define IZ_DOOM_LITE_H
#include <stddef.h>
#include <stdint.h>
#define DOOM_LITE_WIDTH 320
#define DOOM_LITE_HEIGHT 200
#define DOOM_LITE_HEAP_BYTES (12u * 1024u * 1024u)
enum { DL_UP=1u, DL_DOWN=2u, DL_LEFT=4u, DL_RIGHT=8u, DL_FIRE=16u, DL_USE=32u, DL_ENTER=64u, DL_MENU=128u, DL_STRAFE=256u };
typedef struct {
 const uint8_t *wad;
 size_t wad_bytes;
 void *heap;
 size_t heap_bytes;
 void (*print)(const char *);
 /* Must leave the engine via the embedding runtime's error boundary. */
 void (*panic)(int);
} DoomLiteConfig;
typedef struct { const uint8_t *indexed; const uint8_t *palette; uint32_t width,height,number; } DoomLiteFrame;
int doom_lite_boot(const DoomLiteConfig *config);
const DoomLiteFrame *doom_lite_step_timed(uint32_t buttons,uint32_t logicalTick);
const char *doom_lite_error(void);
size_t doom_lite_heap_peak(void);
#endif
