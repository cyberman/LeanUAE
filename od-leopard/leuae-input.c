/*
 * LeanUAE
 * leuae-input.c — Keyboard + Mouse via Carbon Event-System
 *
 * Verantwortung:
 *   - Tastatur-Events von Carbon übersetzen in hostif_input_event
 *   - Maus-Bewegung und Maus-Buttons erfassen
 *   - Maus-Capture (Grab) für den Emulations-Modus
 *   - Events in eine interne Queue legen für den Haupt-Loop
 *
 * Leopard-native Strategie:
 *   - Carbon GetNextEvent / EventRef
 *   - Kein SDL_events, kein HID-Framework für Grundfunktionen
 *   - IOKit für Maus-Capture (CGAssociateMouseAndMouseCursorPosition)
 *
 * Schnittstellen-Vertrag:
 *   - leuae_input_init()    : Event-System vorbereiten
 *   - leuae_input_poll()    : Nächsten Event holen (non-blocking)
 *   - leuae_input_set_grab(): Maus einfangen / freigeben
 *   - leuae_input_close()   : Aufräumen
 *
 * STATUS: Phase 1 — Event-Infrastruktur + Keyboard implementiert
 *         Phase 2 — Maus-Delta, Grab: TODO
 */

#include <stdio.h>
#include <string.h>

#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>

/* ------------------------------------------------------------------ */
/* Event-Typen (identisch mit HOST_INTERFACE.md hostif_input_event)    */
/* ------------------------------------------------------------------ */

typedef enum {
    LEUAE_INPUT_NONE = 0,
    LEUAE_INPUT_KEY_DOWN,
    LEUAE_INPUT_KEY_UP,
    LEUAE_INPUT_MOUSE_MOVE,
    LEUAE_INPUT_MOUSE_BUTTON_DOWN,
    LEUAE_INPUT_MOUSE_BUTTON_UP,
    LEUAE_INPUT_QUIT
} leuae_input_type;

typedef struct {
    leuae_input_type type;
    int              code;       /* Key-Code oder Maustaste */
    int              x, y;      /* Maus-Position (absolut) */
    int              dx, dy;    /* Maus-Delta */
    int              button;    /* Maustaste: 0=links, 1=rechts, 2=mitte */
    unsigned int     modifiers; /* Shift, Ctrl, Alt, Amiga */
} leuae_input_event;

/* ------------------------------------------------------------------ */
/* Interne Event-Queue                                                  */
/* ------------------------------------------------------------------ */

#define LEUAE_INPUT_QUEUE_SIZE 64

static leuae_input_event g_queue[LEUAE_INPUT_QUEUE_SIZE];
static int               g_queue_head = 0;
static int               g_queue_tail = 0;
static int               g_grab       = 0;

static int g_last_mouse_x = 0;
static int g_last_mouse_y = 0;

/* ------------------------------------------------------------------ */
/* Queue-Hilfsfunktionen                                               */
/* ------------------------------------------------------------------ */

static int queue_push(const leuae_input_event *ev)
{
    int next = (g_queue_tail + 1) % LEUAE_INPUT_QUEUE_SIZE;
    if (next == g_queue_head) {
        return 0;  /* Queue voll */
    }
    g_queue[g_queue_tail] = *ev;
    g_queue_tail = next;
    return 1;
}

static int queue_pop(leuae_input_event *ev)
{
    if (g_queue_head == g_queue_tail) {
        return 0;  /* Queue leer */
    }
    *ev = g_queue[g_queue_head];
    g_queue_head = (g_queue_head + 1) % LEUAE_INPUT_QUEUE_SIZE;
    return 1;
}

/* ------------------------------------------------------------------ */
/* Key-Code Übersetzung: Carbon VK → Amiga-Key                        */
/* ------------------------------------------------------------------ */

/*
 * Minimale Tabelle der wichtigsten Tasten.
 * Carbon Virtual Key Codes → UAE-interne Keycodes.
 * Vollständige Tabelle folgt in Phase 2.
 *
 * Amiga-Keycodes nach: Hardware Reference Manual, Appendix A.
 */
static int carbon_vk_to_amiga(int vk)
{
    switch (vk) {
        /* Buchstaben */
        case kVK_ANSI_A: return 0x20;
        case kVK_ANSI_B: return 0x35;
        case kVK_ANSI_C: return 0x33;
        case kVK_ANSI_D: return 0x22;
        case kVK_ANSI_E: return 0x12;
        case kVK_ANSI_F: return 0x23;
        case kVK_ANSI_G: return 0x24;
        case kVK_ANSI_H: return 0x25;
        case kVK_ANSI_I: return 0x17;
        case kVK_ANSI_J: return 0x26;
        case kVK_ANSI_K: return 0x27;
        case kVK_ANSI_L: return 0x28;
        case kVK_ANSI_M: return 0x37;
        case kVK_ANSI_N: return 0x36;
        case kVK_ANSI_O: return 0x18;
        case kVK_ANSI_P: return 0x19;
        case kVK_ANSI_Q: return 0x10;
        case kVK_ANSI_R: return 0x13;
        case kVK_ANSI_S: return 0x21;
        case kVK_ANSI_T: return 0x14;
        case kVK_ANSI_U: return 0x16;
        case kVK_ANSI_V: return 0x34;
        case kVK_ANSI_W: return 0x11;
        case kVK_ANSI_X: return 0x32;
        case kVK_ANSI_Y: return 0x15;
        case kVK_ANSI_Z: return 0x31;
        /* Ziffern */
        case kVK_ANSI_1: return 0x01;
        case kVK_ANSI_2: return 0x02;
        case kVK_ANSI_3: return 0x03;
        case kVK_ANSI_4: return 0x04;
        case kVK_ANSI_5: return 0x05;
        case kVK_ANSI_6: return 0x06;
        case kVK_ANSI_7: return 0x07;
        case kVK_ANSI_8: return 0x08;
        case kVK_ANSI_9: return 0x09;
        case kVK_ANSI_0: return 0x0A;
        /* Steuerung */
        case kVK_Return:        return 0x44;
        case kVK_Space:         return 0x40;
        case kVK_Delete:        return 0x41;  /* Backspace */
        case kVK_Escape:        return 0x45;
        case kVK_Tab:           return 0x42;
        case kVK_UpArrow:       return 0x4C;
        case kVK_DownArrow:     return 0x4D;
        case kVK_LeftArrow:     return 0x4F;
        case kVK_RightArrow:    return 0x4E;
        /* Funktionstasten */
        case kVK_F1:  return 0x50;
        case kVK_F2:  return 0x51;
        case kVK_F3:  return 0x52;
        case kVK_F4:  return 0x53;
        case kVK_F5:  return 0x54;
        case kVK_F6:  return 0x55;
        case kVK_F7:  return 0x56;
        case kVK_F8:  return 0x57;
        case kVK_F9:  return 0x58;
        case kVK_F10: return 0x59;
        default:      return -1;  /* Unbekannte Taste */
    }
}

/*
 * carbon_modifiers_to_flags()
 *
 * Übersetzt Carbon-Modifier-Bits in LeanUAE-interne Flags.
 */
static unsigned int carbon_modifiers_to_flags(UInt32 mods)
{
    unsigned int flags = 0;
    if (mods & shiftKey)    flags |= 0x01;
    if (mods & controlKey)  flags |= 0x02;
    if (mods & optionKey)   flags |= 0x04;  /* Alt / Amiga */
    if (mods & cmdKey)      flags |= 0x08;  /* Amiga */
    return flags;
}

/* ------------------------------------------------------------------ */
/* Öffentliche Schnittstelle                                            */
/* ------------------------------------------------------------------ */

/*
 * leuae_input_init()
 *
 * Bereitet das Event-System vor.
 * Auf Leopard: Nichts weiter nötig — Carbon ist immer verfügbar.
 *
 * Gibt 0 zurück bei Erfolg.
 */
int leuae_input_init(void)
{
    g_queue_head = 0;
    g_queue_tail = 0;
    g_grab       = 0;
    g_last_mouse_x = 0;
    g_last_mouse_y = 0;

    fprintf(stderr, "leuae-input: init\n");
    return 0;
}

/*
 * leuae_input_pump()
 *
 * Verarbeitet ausstehende Carbon-Events und legt sie in die interne Queue.
 * Muss einmal pro Hauptschleifendurchlauf aufgerufen werden.
 *
 * Gibt die Anzahl verarbeiteter Events zurück.
 */
int leuae_input_pump(void)
{
    EventRef        event_ref;
    OSStatus        err;
    UInt32          event_class;
    UInt32          event_kind;
    leuae_input_event ev;
    int             count = 0;
    Point           mouse_pt;
    UInt32          key_code;
    UInt32          modifiers;
    int             amiga_key;

    while (1) {
        err = ReceiveNextEvent(0, NULL,
                               kEventDurationNoWait, /* non-blocking */
                               true,                 /* pull from queue */
                               &event_ref);

        if (err == eventLoopTimedOutErr || err != noErr) {
            break;  /* Keine weiteren Events */
        }

        event_class = GetEventClass(event_ref);
        event_kind  = GetEventKind(event_ref);

        memset(&ev, 0, sizeof(ev));

        switch (event_class) {

        case kEventClassKeyboard:
            GetEventParameter(event_ref,
                              kEventParamKeyCode,
                              typeUInt32, NULL,
                              sizeof(key_code), NULL,
                              &key_code);
            GetEventParameter(event_ref,
                              kEventParamKeyModifiers,
                              typeUInt32, NULL,
                              sizeof(modifiers), NULL,
                              &modifiers);

            amiga_key = carbon_vk_to_amiga((int)key_code);
            if (amiga_key >= 0) {
                ev.code      = amiga_key;
                ev.modifiers = carbon_modifiers_to_flags(modifiers);
                ev.type = (event_kind == kEventRawKeyDown)
                          ? LEUAE_INPUT_KEY_DOWN
                          : LEUAE_INPUT_KEY_UP;
                queue_push(&ev);
                count++;
            }
            break;

        case kEventClassMouse:
            GetEventParameter(event_ref,
                              kEventParamMouseLocation,
                              typeQDPoint, NULL,
                              sizeof(mouse_pt), NULL,
                              &mouse_pt);

            if (event_kind == kEventMouseMoved ||
                event_kind == kEventMouseDragged) {

                ev.type = LEUAE_INPUT_MOUSE_MOVE;
                ev.x    = mouse_pt.h;
                ev.y    = mouse_pt.v;
                ev.dx   = mouse_pt.h - g_last_mouse_x;
                ev.dy   = mouse_pt.v - g_last_mouse_y;
                g_last_mouse_x = mouse_pt.h;
                g_last_mouse_y = mouse_pt.v;
                queue_push(&ev);
                count++;

            } else if (event_kind == kEventMouseDown ||
                       event_kind == kEventMouseUp) {

                EventMouseButton btn;
                GetEventParameter(event_ref,
                                  kEventParamMouseButton,
                                  typeMouseButton, NULL,
                                  sizeof(btn), NULL,
                                  &btn);

                ev.type   = (event_kind == kEventMouseDown)
                            ? LEUAE_INPUT_MOUSE_BUTTON_DOWN
                            : LEUAE_INPUT_MOUSE_BUTTON_UP;
                ev.x      = mouse_pt.h;
                ev.y      = mouse_pt.v;
                /* Carbon: kEventMouseButtonPrimary=1, Secondary=2, Tertiary=3
                 * Wir normalisieren auf 0-basiert */
                ev.button = (int)btn - 1;
                queue_push(&ev);
                count++;
            }
            break;

        case kEventClassApplication:
            if (event_kind == kEventAppQuit) {
                ev.type = LEUAE_INPUT_QUIT;
                queue_push(&ev);
                count++;
            }
            break;

        default:
            break;
        }

        /* Event an das System weitergeben (für Standard-Behandlung) */
        SendEventToEventTarget(event_ref, GetEventDispatcherTarget());
        ReleaseEvent(event_ref);
    }

    return count;
}

/*
 * leuae_input_poll()
 *
 * Holt den nächsten Event aus der internen Queue.
 * Non-blocking — gibt 0 zurück wenn Queue leer.
 *
 * ev wird nur geschrieben wenn 1 zurückgegeben wird.
 */
int leuae_input_poll(leuae_input_event *ev)
{
    if (!ev) return 0;
    return queue_pop(ev);
}

/*
 * leuae_input_set_grab()
 *
 * Fängt die Maus ein (enabled=1) oder gibt sie frei (enabled=0).
 * Im Grab-Modus wird der Cursor ausgeblendet und Maus-Bewegung
 * wird relativ gemeldet — nötig für Amiga-Maus-Emulation.
 */
void leuae_input_set_grab(int enabled)
{
    if (enabled == g_grab) return;

    g_grab = enabled;

    if (enabled) {
        /* Cursor ausblenden */
        CGDisplayHideCursor(kCGDirectMainDisplay);
        /* Maus von Cursor-Position entkoppeln */
        CGAssociateMouseAndMouseCursorPosition(false);
    } else {
        /* Cursor wieder zeigen */
        CGDisplayShowCursor(kCGDirectMainDisplay);
        CGAssociateMouseAndMouseCursorPosition(true);
    }
}

/*
 * leuae_input_close()
 *
 * Gibt Grab frei und räumt auf.
 */
void leuae_input_close(void)
{
    if (g_grab) {
        leuae_input_set_grab(0);
    }
    g_queue_head = 0;
    g_queue_tail = 0;
    fprintf(stderr, "leuae-input: geschlossen\n");
}
