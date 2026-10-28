#ifndef DOOM_RASTER_ABI_H
#define DOOM_RASTER_ABI_H
#define RASTER_STATE 0x100u
#define RASTER_QUEUE 0x2000u
#define RASTER_WORDS 10u
#define RASTER_CAPACITY 1024u
/* State words: count, pending, flush sequence, completed sequence, error. */
/* Command words: op, destination, source, colormap, count, a,b,c,d,reserved.
 * Column: a=initial fixed frac, b=step; destination rows stride 320.
 * Span: a=xfrac,b=yfrac,c=xstep,d=ystep; destination bytes contiguous.
 * All arithmetic is the original modulo-2^32 fixed-point arithmetic.
 */
#endif
