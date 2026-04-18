/*
 * LeUAE - Leopard UAE
 * leuae-video.c — Video/Framebuffer via Quartz 2D / AGL
 * Ersetzt: od-fs/fsemu/fsemu-sdlvideo.c
 * STATUS: STUB
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/* TODO: #include <CoreGraphics/CoreGraphics.h> */
/* TODO: #include <AGL/agl.h> */

static uint8_t *g_render_buffer = NULL;
static int g_width  = 0;
static int g_height = 0;

uint8_t *uae_get_render_buffer(void)
{
    /* TODO: Quartz CGBitmapContext als Backing-Store */
    return g_render_buffer;
}

int leuae_video_init(int width, int height)
{
    g_width  = width;
    g_height = height;
    /* TODO: CGBitmapContextCreate */
    fprintf(stderr, "leuae-video: STUB init %dx%d\n", width, height);
    return 0;
}

void leuae_video_show_frame(void)
{
    /* TODO: CGContextDrawImage */
}

void leuae_video_close(void)
{
    free(g_render_buffer);
    g_render_buffer = NULL;
}
