/*
 * LeanUAE
 * leuae-video.c — Framebuffer via CoreGraphics
 *
 * Verantwortung:
 *   - Bereitstellen eines nativen Leopard-Render-Buffers
 *   - Präsentieren fertiger Frames in das Leopard-Fenster
 *
 * Abhängigkeit:
 *   - leuae-window.c : liefert den CGContextRef zum Zeichnen
 *
 * Leopard-native Strategie:
 *   - CGBitmapContext als Backing-Store (Pixel-Owner)
 *   - kCGBitmapByteOrder32Big: PPC-nativ, kein Byte-Swap
 *   - Pixel-Format: ARGB8888 (32 Bit pro Pixel)
 *   - CGContextDrawImage zum Präsentieren in den Window-Context
 *
 * Schnittstellen-Vertrag:
 *   - leuae_video_init()      : Buffer anlegen
 *   - leuae_video_get_buf()   : Zeiger auf Pixel-Speicher liefern
 *   - leuae_video_get_pitch() : Stride in Bytes liefern
 *   - leuae_video_show()      : Frame in Fenster präsentieren
 *   - leuae_video_close()     : Ressourcen freigeben
 *
 * STATUS: Phase 2 — Buffer + Präsentation implementiert
 */

#include <stdint.h>
#include <stdio.h>

#include <CoreGraphics/CoreGraphics.h>

/* Vorwärts-Deklaration aus leuae-window.c */
extern CGContextRef leuae_window_get_context(void);
extern void         leuae_window_flush(void);

/* ------------------------------------------------------------------ */
/* Interner Zustand                                                     */
/* ------------------------------------------------------------------ */

static CGContextRef  g_bitmap_ctx = NULL;
static uint8_t      *g_pixels     = NULL;
static int           g_width      = 0;
static int           g_height     = 0;
static size_t        g_pitch      = 0;

/* Pixel-Format */
#define LEUAE_BYTES_PER_PIXEL  4
#define LEUAE_BITS_PER_CHANNEL 8

/* ------------------------------------------------------------------ */
/* Öffentliche Schnittstelle                                            */
/* ------------------------------------------------------------------ */

/*
 * leuae_video_init()
 *
 * Legt einen CGBitmapContext als nativen Pixel-Backing-Store an.
 * CoreGraphics verwaltet den Speicher intern (NULL als data-Zeiger).
 * ARGB8888 Big-Endian — PPC-native, kein Byte-Swap nötig.
 *
 * Gibt 0 zurück bei Erfolg, -1 bei Fehler.
 */
int leuae_video_init(int width, int height)
{
    CGColorSpaceRef colorspace;

    if (width <= 0 || height <= 0) {
        fprintf(stderr, "leuae-video: ungueltige Dimensionen %dx%d\n",
                width, height);
        return -1;
    }

    colorspace = CGColorSpaceCreateDeviceRGB();
    if (!colorspace) {
        fprintf(stderr, "leuae-video: CGColorSpaceCreateDeviceRGB fehlgeschlagen\n");
        return -1;
    }

    g_pitch = (size_t)width * LEUAE_BYTES_PER_PIXEL;

    g_bitmap_ctx = CGBitmapContextCreate(
        NULL,
        (size_t)width,
        (size_t)height,
        LEUAE_BITS_PER_CHANNEL,
        g_pitch,
        colorspace,
        kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedFirst
    );

    CGColorSpaceRelease(colorspace);

    if (!g_bitmap_ctx) {
        fprintf(stderr, "leuae-video: CGBitmapContextCreate fehlgeschlagen\n");
        g_pitch = 0;
        return -1;
    }

    g_pixels = (uint8_t *)CGBitmapContextGetData(g_bitmap_ctx);
    g_width  = width;
    g_height = height;

    fprintf(stderr, "leuae-video: init %dx%d, pitch=%zu, pixels=%p\n",
            g_width, g_height, g_pitch, (void *)g_pixels);

    return 0;
}

/*
 * leuae_video_get_buf()
 *
 * Liefert den Zeiger auf den Pixel-Speicher.
 * Der UAE-Core schreibt fertige Frames direkt hier hinein.
 */
uint8_t *leuae_video_get_buf(void)
{
    return g_pixels;
}

/*
 * leuae_video_get_pitch()
 *
 * Liefert die Zeilenbreite in Bytes.
 * Wird vom Core benötigt um korrekt in den Buffer zu schreiben.
 */
size_t leuae_video_get_pitch(void)
{
    return g_pitch;
}

/*
 * leuae_video_show()
 *
 * Präsentiert den aktuellen Frame im Leopard-Fenster.
 *
 * Ablauf:
 *   1. CGImage aus dem Bitmap-Context erzeugen (Zero-Copy-Referenz)
 *   2. Window-Context von leuae-window.c holen
 *   3. CGImage in den Window-Context zeichnen
 *   4. Window-Context flushen und freigeben
 */
void leuae_video_show(void)
{
    CGImageRef   img;
    CGContextRef win_ctx;
    CGRect       dst;

    if (!g_bitmap_ctx) {
        return;
    }

    /* CGImage aus Bitmap-Context — keine Kopie, nur Referenz */
    img = CGBitmapContextCreateImage(g_bitmap_ctx);
    if (!img) {
        fprintf(stderr, "leuae-video: CGBitmapContextCreateImage fehlgeschlagen\n");
        return;
    }

    /* Window-Context holen */
    win_ctx = leuae_window_get_context();
    if (!win_ctx) {
        CGImageRelease(img);
        return;
    }

    /*
     * Koordinaten-System: CoreGraphics hat Ursprung unten-links.
     * CGContextDrawImage zeichnet das Bild in das angegebene Rechteck.
     * Wir füllen das gesamte Fenster.
     */
    dst = CGRectMake(0.0, 0.0, (CGFloat)g_width, (CGFloat)g_height);
    CGContextDrawImage(win_ctx, dst, img);

    CGImageRelease(img);

    /* Context flushen — bringt den Frame auf den Bildschirm */
    leuae_window_flush();
}

/*
 * leuae_video_close()
 *
 * Gibt alle Video-Ressourcen frei.
 * Nach diesem Aufruf ist der Zeiger aus leuae_video_get_buf() ungültig.
 */
void leuae_video_close(void)
{
    if (g_bitmap_ctx) {
        CGContextRelease(g_bitmap_ctx);
        g_bitmap_ctx = NULL;
    }

    g_pixels = NULL;
    g_width  = 0;
    g_height = 0;
    g_pitch  = 0;
}
