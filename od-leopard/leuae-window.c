/*
 * LeanUAE
 * leuae-window.c — Natives Leopard-Fenster via Carbon
 *
 * Verantwortung:
 *   - Ein natives Leopard-Fenster erzeugen und verwalten
 *   - Window-Context für leuae-video.c bereitstellen
 *   - Fullscreen-Toggle
 *   - Ordentliches Schließen
 *
 * Leopard-native Strategie:
 *   - Carbon CreateNewWindow / WindowRef
 *   - Kein Cocoa / kein NSWindow — Carbon ist auf PPC Leopard
 *     die direkteste, stabilste Wahl ohne ObjC-Runtime-Overhead
 *   - QuitApplicationEventLoop() als sauberer Ausstiegspunkt
 *
 * Schnittstellen-Vertrag:
 *   - leuae_window_init()        : Fenster erzeugen, Event-Loop vorbereiten
 *   - leuae_window_get_context() : CGContextRef für leuae-video liefern
 *   - leuae_window_set_title()   : Fenstertitel setzen
 *   - leuae_window_flush()       : Fenster-Inhalt auf Bildschirm bringen
 *   - leuae_window_close()       : Ressourcen freigeben
 *
 * STATUS: Phase 1 — Fenster-Erzeugung implementiert
 *         Phase 2 — Event-Pump, Fullscreen: TODO
 */

#include <stdio.h>
#include <string.h>

#include <Carbon/Carbon.h>

/* ------------------------------------------------------------------ */
/* Interner Zustand                                                     */
/* ------------------------------------------------------------------ */

static WindowRef  g_window     = NULL;
static CGContextRef g_cg_ctx   = NULL;
static int        g_width      = 0;
static int        g_height     = 0;
static int        g_fullscreen = 0;

/* ------------------------------------------------------------------ */
/* Hilfsfunktionen                                                      */
/* ------------------------------------------------------------------ */

/*
 * window_get_cg_context()
 *
 * Holt den aktuellen CoreGraphics-Context aus dem Carbon-Fenster.
 * Muss vor jedem Zeichnen aufgerufen werden — der Context kann
 * nach Fenster-Events ungültig werden.
 */
static CGContextRef window_get_cg_context(void)
{
    CGContextRef ctx = NULL;

    if (!g_window) {
        return NULL;
    }

    /* Carbon-Weg: QDBeginCGContext liefert einen CG-Context
     * der direkt in den Fenster-Backing-Store zeichnet.
     * Auf Leopard PPC ist das der stabilste Pfad ohne OpenGL. */
    if (QDBeginCGContext(GetWindowPort(g_window), &ctx) != noErr) {
        fprintf(stderr, "leuae-window: QDBeginCGContext fehlgeschlagen\n");
        return NULL;
    }

    return ctx;
}

/* ------------------------------------------------------------------ */
/* Öffentliche Schnittstelle                                            */
/* ------------------------------------------------------------------ */

/*
 * leuae_window_init()
 *
 * Erzeugt ein natives Carbon-Fenster in der angegebenen Größe.
 * Das Fenster ist zunächst unsichtbar — ShowWindow() folgt am Ende.
 *
 * Gibt 0 zurück bei Erfolg, -1 bei Fehler.
 */
int leuae_window_init(int width, int height, const char *title)
{
    Rect         bounds;
    OSStatus     err;
    WindowAttributes attrs;

    if (width <= 0 || height <= 0) {
        fprintf(stderr, "leuae-window: ungueltige Dimensionen %dx%d\n",
                width, height);
        return -1;
    }

    /* Fenster-Rechteck: oben-links bei (60,60), Größe = width x height */
    SetRect(&bounds, 60, 60, 60 + width, 60 + height);

    attrs = kWindowStandardHandlerAttribute
          | kWindowCloseBoxAttribute
          | kWindowCollapseBoxAttribute;

    err = CreateNewWindow(kDocumentWindowClass, attrs, &bounds, &g_window);
    if (err != noErr || !g_window) {
        fprintf(stderr, "leuae-window: CreateNewWindow fehlgeschlagen (%d)\n",
                (int)err);
        return -1;
    }

    g_width  = width;
    g_height = height;

    /* Titel setzen */
    if (title && title[0]) {
        leuae_window_set_title(title);
    }

    /* Fenster sichtbar machen */
    ShowWindow(g_window);
    SelectWindow(g_window);

    fprintf(stderr, "leuae-window: Fenster erstellt %dx%d\n", width, height);
    return 0;
}

/*
 * leuae_window_set_title()
 *
 * Setzt den Fenstertitel als Pascal-String (Carbon-Konvention).
 */
void leuae_window_set_title(const char *title)
{
    CFStringRef cf_title;

    if (!g_window || !title) {
        return;
    }

    cf_title = CFStringCreateWithCString(NULL, title, kCFStringEncodingUTF8);
    if (cf_title) {
        SetWindowTitleWithCFString(g_window, cf_title);
        CFRelease(cf_title);
    }
}

/*
 * leuae_window_get_context()
 *
 * Liefert einen CGContextRef der direkt in das Fenster zeichnet.
 * Wird von leuae_video_show() genutzt um den Framebuffer zu präsentieren.
 *
 * WICHTIG: Nach dem Zeichnen muss leuae_window_flush() aufgerufen werden
 * um den Context ordentlich zu schließen (QDEndCGContext).
 */
CGContextRef leuae_window_get_context(void)
{
    if (g_cg_ctx) {
        /* Vorherigen Context sauber schließen bevor neuer geholt wird */
        QDEndCGContext(GetWindowPort(g_window), &g_cg_ctx);
        g_cg_ctx = NULL;
    }

    g_cg_ctx = window_get_cg_context();
    return g_cg_ctx;
}

/*
 * leuae_window_flush()
 *
 * Schließt den aktuellen CG-Context und bringt den Fensterinhalt
 * auf den Bildschirm. Muss nach jedem Frame aufgerufen werden.
 */
void leuae_window_flush(void)
{
    if (!g_window || !g_cg_ctx) {
        return;
    }

    CGContextFlush(g_cg_ctx);
    QDEndCGContext(GetWindowPort(g_window), &g_cg_ctx);
    g_cg_ctx = NULL;
}

/*
 * leuae_window_close()
 *
 * Gibt alle Fenster-Ressourcen frei.
 * Muss vor Programmende aufgerufen werden.
 */
void leuae_window_close(void)
{
    if (g_cg_ctx && g_window) {
        QDEndCGContext(GetWindowPort(g_window), &g_cg_ctx);
        g_cg_ctx = NULL;
    }

    if (g_window) {
        DisposeWindow(g_window);
        g_window = NULL;
    }

    g_width      = 0;
    g_height     = 0;
    g_fullscreen = 0;

    fprintf(stderr, "leuae-window: geschlossen\n");
}
