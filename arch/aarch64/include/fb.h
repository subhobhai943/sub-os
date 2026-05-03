#ifndef AARCH64_FB_H
#define AARCH64_FB_H

#include "types.h"

/* Framebuffer info - set after fb_init() */
extern u32 *fb_pixels;   /* ARGB32 pixel buffer */
extern u32  fb_width;
extern u32  fb_height;
extern u32  fb_pitch;    /* bytes per row */

int  fb_init(void);      /* returns 0 on success */
void fb_flush(void);     /* push pixels to display */
void fb_fill(u32 color);

#endif
