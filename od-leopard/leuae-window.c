/*
 * LeUAE - Leopard UAE
 * leuae-window.c — Fensterverwaltung via Quartz / Carbon
 * Ersetzt: od-fs/fsemu/fsemu-sdlwindow.c (53 KB in FS-UAE!)
 * STATUS: STUB
 */

#include <stdint.h>
#include <stdio.h>

/* TODO: #include <Carbon/Carbon.h> */

static int g_window_width  = 752;
static int g_window_height = 574;

int leuae_window_create(const char *title, int width, int height)
{
    g_window_width  = width;
    g_window_height = height;
    /* TODO: CreateNewWindow / ShowWindow */
    fprintf(stderr, "leuae-window: STUB create '%s' %dx%d\n", title, width, height);
    return 0;
}

void leuae_window_close(void)
{
    /* TODO: DisposeWindow */
}

int leuae_window_get_width(void)  { return g_window_width;  }
int leuae_window_get_height(void) { return g_window_height; }
