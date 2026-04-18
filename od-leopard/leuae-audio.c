/*
 * LeanUAE
 * leuae-audio.c — Audio-Output via CoreAudio AudioQueue
 *
 * Verantwortung:
 *   - PCM-Audio-Ausgabe über Leopard-native CoreAudio
 *   - Double-Buffering via AudioQueue (zwei Puffer im Ring)
 *   - Unterrun-Behandlung (Stille statt Knacken)
 *
 * Leopard-native Strategie:
 *   - AudioQueue ist die high-level CoreAudio API auf Leopard
 *   - Kein SDL_audio, kein PortAudio, kein OpenAL
 *   - Callback-Modell: CoreAudio ruft uns wenn Daten gebraucht werden
 *   - Signed 16-bit PCM, Stereo, 44100 Hz (UAE-Standard)
 *
 * Schnittstellen-Vertrag:
 *   - leuae_audio_init()    : Queue + Buffer anlegen, Ausgabe starten
 *   - leuae_audio_submit()  : PCM-Samples vom Core einreichen
 *   - leuae_audio_stop()    : Ausgabe pausieren
 *   - leuae_audio_close()   : Ressourcen freigeben
 *
 * STATUS: Phase 1 — AudioQueue-Infrastruktur implementiert
 */

#include <stdio.h>
#include <string.h>

#include <AudioToolbox/AudioToolbox.h>

/* ------------------------------------------------------------------ */
/* Konfiguration                                                        */
/* ------------------------------------------------------------------ */

#define LEUAE_AUDIO_SAMPLE_RATE    44100
#define LEUAE_AUDIO_CHANNELS       2
#define LEUAE_AUDIO_BITS           16
#define LEUAE_AUDIO_BYTES_PER_FRAME \
    (LEUAE_AUDIO_CHANNELS * (LEUAE_AUDIO_BITS / 8))  /* 4 */

/* Puffer-Größe: ~23ms bei 44100 Hz — klein genug für Latenz,
 * groß genug um Underruns auf dem G5 zu vermeiden            */
#define LEUAE_AUDIO_FRAMES_PER_BUF  1024
#define LEUAE_AUDIO_BUF_SIZE \
    (LEUAE_AUDIO_FRAMES_PER_BUF * LEUAE_AUDIO_BYTES_PER_FRAME)

/* Anzahl der Queue-Buffer (Double-Buffering) */
#define LEUAE_AUDIO_NUM_BUFS  2

/* ------------------------------------------------------------------ */
/* Interner Zustand                                                     */
/* ------------------------------------------------------------------ */

static AudioQueueRef       g_queue     = NULL;
static AudioQueueBufferRef g_bufs[LEUAE_AUDIO_NUM_BUFS];
static int                 g_running   = 0;

/*
 * Ring-Buffer für eingehende PCM-Daten vom UAE-Core.
 * Einfaches Single-Producer / Single-Consumer Modell:
 * - Producer: leuae_audio_submit() (Emulations-Thread)
 * - Consumer: audio_callback()     (CoreAudio-Thread)
 */
#define LEUAE_RING_FRAMES   8192
#define LEUAE_RING_SIZE     (LEUAE_RING_FRAMES * LEUAE_AUDIO_BYTES_PER_FRAME)

static uint8_t  g_ring[LEUAE_RING_SIZE];
static volatile int g_ring_write = 0;  /* in Bytes */
static volatile int g_ring_read  = 0;  /* in Bytes */

/* ------------------------------------------------------------------ */
/* Ring-Buffer Hilfsfunktionen                                          */
/* ------------------------------------------------------------------ */

static int ring_available(void)
{
    int w = g_ring_write;
    int r = g_ring_read;
    if (w >= r) return w - r;
    return LEUAE_RING_SIZE - r + w;
}

static void ring_write(const uint8_t *src, int bytes)
{
    int w = g_ring_write;
    int first = LEUAE_RING_SIZE - w;

    if (first >= bytes) {
        memcpy(g_ring + w, src, bytes);
    } else {
        memcpy(g_ring + w, src, first);
        memcpy(g_ring, src + first, bytes - first);
    }
    g_ring_write = (w + bytes) % LEUAE_RING_SIZE;
}

static void ring_read(uint8_t *dst, int bytes)
{
    int r = g_ring_read;
    int first = LEUAE_RING_SIZE - r;

    if (first >= bytes) {
        memcpy(dst, g_ring + r, bytes);
    } else {
        memcpy(dst, g_ring + r, first);
        memcpy(dst + first, g_ring, bytes - first);
    }
    g_ring_read = (r + bytes) % LEUAE_RING_SIZE;
}

/* ------------------------------------------------------------------ */
/* CoreAudio Callback                                                   */
/* ------------------------------------------------------------------ */

/*
 * audio_callback()
 *
 * Wird von CoreAudio aufgerufen wenn ein Buffer leer ist und
 * neue Daten gebraucht werden. Läuft auf CoreAudio-eigenem Thread.
 *
 * Wir füllen den Buffer aus dem Ring-Buffer.
 * Bei Unterrun: Stille (Zero-Fill) statt Knacken.
 */
static void audio_callback(void                 *user_data,
                            AudioQueueRef         queue,
                            AudioQueueBufferRef   buffer)
{
    int   needed = (int)buffer->mAudioDataBytesCapacity;
    int   avail  = ring_available();
    int   fill   = avail < needed ? avail : needed;

    (void)user_data;

    if (fill > 0) {
        ring_read((uint8_t *)buffer->mAudioData, fill);
    }

    /* Rest mit Stille füllen bei Unterrun */
    if (fill < needed) {
        memset((uint8_t *)buffer->mAudioData + fill, 0, needed - fill);
        if (fill == 0) {
            /* Vollständiger Unterrun — nur bei Debug relevant */
            /* fprintf(stderr, "leuae-audio: underrun\n"); */
        }
    }

    buffer->mAudioDataByteSize = (UInt32)needed;
    AudioQueueEnqueueBuffer(queue, buffer, 0, NULL);
}

/* ------------------------------------------------------------------ */
/* Öffentliche Schnittstelle                                            */
/* ------------------------------------------------------------------ */

/*
 * leuae_audio_init()
 *
 * Richtet eine CoreAudio AudioQueue für PCM-Ausgabe ein.
 * Startet die Ausgabe sofort (Stille bis erste Samples eintreffen).
 *
 * Gibt 0 zurück bei Erfolg, -1 bei Fehler.
 */
int leuae_audio_init(void)
{
    AudioStreamBasicDescription fmt;
    OSStatus err;
    int i;

    /* PCM-Format definieren */
    memset(&fmt, 0, sizeof(fmt));
    fmt.mSampleRate       = LEUAE_AUDIO_SAMPLE_RATE;
    fmt.mFormatID         = kAudioFormatLinearPCM;
    fmt.mFormatFlags      = kLinearPCMFormatFlagIsSignedInteger
                          | kLinearPCMFormatFlagIsPacked
                          | kAudioFormatFlagIsBigEndian;  /* PPC-native */
    fmt.mBytesPerPacket   = LEUAE_AUDIO_BYTES_PER_FRAME;
    fmt.mFramesPerPacket  = 1;
    fmt.mBytesPerFrame    = LEUAE_AUDIO_BYTES_PER_FRAME;
    fmt.mChannelsPerFrame = LEUAE_AUDIO_CHANNELS;
    fmt.mBitsPerChannel   = LEUAE_AUDIO_BITS;

    /* Output-Queue anlegen */
    err = AudioQueueNewOutput(&fmt, audio_callback, NULL,
                              NULL, NULL, 0, &g_queue);
    if (err != noErr) {
        fprintf(stderr, "leuae-audio: AudioQueueNewOutput fehlgeschlagen (%d)\n",
                (int)err);
        return -1;
    }

    /* Buffer allokieren und vorbelegen */
    for (i = 0; i < LEUAE_AUDIO_NUM_BUFS; i++) {
        err = AudioQueueAllocateBuffer(g_queue, LEUAE_AUDIO_BUF_SIZE,
                                       &g_bufs[i]);
        if (err != noErr) {
            fprintf(stderr, "leuae-audio: AudioQueueAllocateBuffer[%d] "
                    "fehlgeschlagen (%d)\n", i, (int)err);
            AudioQueueDispose(g_queue, true);
            g_queue = NULL;
            return -1;
        }

        /* Mit Stille vorbelegen und einreihen */
        memset(g_bufs[i]->mAudioData, 0, LEUAE_AUDIO_BUF_SIZE);
        g_bufs[i]->mAudioDataByteSize = LEUAE_AUDIO_BUF_SIZE;
        AudioQueueEnqueueBuffer(g_queue, g_bufs[i], 0, NULL);
    }

    /* Ring-Buffer initialisieren */
    memset(g_ring, 0, sizeof(g_ring));
    g_ring_write = 0;
    g_ring_read  = 0;

    /* Ausgabe starten */
    err = AudioQueueStart(g_queue, NULL);
    if (err != noErr) {
        fprintf(stderr, "leuae-audio: AudioQueueStart fehlgeschlagen (%d)\n",
                (int)err);
        AudioQueueDispose(g_queue, true);
        g_queue = NULL;
        return -1;
    }

    g_running = 1;
    fprintf(stderr, "leuae-audio: init %d Hz, %d ch, %d bit, buf=%d frames\n",
            LEUAE_AUDIO_SAMPLE_RATE, LEUAE_AUDIO_CHANNELS,
            LEUAE_AUDIO_BITS, LEUAE_AUDIO_FRAMES_PER_BUF);

    return 0;
}

/*
 * leuae_audio_submit()
 *
 * Nimmt PCM-Samples vom UAE-Core entgegen und legt sie in den Ring-Buffer.
 * samples: Zeiger auf signed 16-bit PCM, interleaved Stereo
 * frame_count: Anzahl Stereo-Frames (je 4 Bytes)
 *
 * Gibt die Anzahl tatsächlich eingeschriebener Frames zurück.
 */
int leuae_audio_submit(const int16_t *samples, int frame_count)
{
    int bytes_total;
    int bytes_free;
    int bytes_write;
    int frames_written;

    if (!g_queue || !g_running || !samples || frame_count <= 0) {
        return 0;
    }

    bytes_total  = frame_count * LEUAE_AUDIO_BYTES_PER_FRAME;
    bytes_free   = LEUAE_RING_SIZE - ring_available() - LEUAE_AUDIO_BYTES_PER_FRAME;
    bytes_write  = bytes_total < bytes_free ? bytes_total : bytes_free;

    if (bytes_write <= 0) {
        /* Ring-Buffer voll — Overrun, Daten verwerfen */
        return 0;
    }

    /* Auf Frame-Grenzen abrunden */
    bytes_write = (bytes_write / LEUAE_AUDIO_BYTES_PER_FRAME)
                * LEUAE_AUDIO_BYTES_PER_FRAME;

    ring_write((const uint8_t *)samples, bytes_write);
    frames_written = bytes_write / LEUAE_AUDIO_BYTES_PER_FRAME;

    return frames_written;
}

/*
 * leuae_audio_get_delay_frames()
 *
 * Gibt die Anzahl der Frames zurück die sich aktuell im Ring-Buffer
 * befinden. Kann vom Core für Audio-Synchronisation genutzt werden.
 */
int leuae_audio_get_delay_frames(void)
{
    return ring_available() / LEUAE_AUDIO_BYTES_PER_FRAME;
}

/*
 * leuae_audio_stop()
 *
 * Pausiert die Audio-Ausgabe (z.B. bei Emulations-Pause).
 */
void leuae_audio_stop(void)
{
    if (g_queue && g_running) {
        AudioQueuePause(g_queue);
        g_running = 0;
    }
}

/*
 * leuae_audio_resume()
 *
 * Setzt die Audio-Ausgabe nach leuae_audio_stop() fort.
 */
void leuae_audio_resume(void)
{
    if (g_queue && !g_running) {
        AudioQueueStart(g_queue, NULL);
        g_running = 1;
    }
}

/*
 * leuae_audio_close()
 *
 * Stoppt die Queue und gibt alle Audio-Ressourcen frei.
 */
void leuae_audio_close(void)
{
    if (g_queue) {
        AudioQueueStop(g_queue, true);  /* true = synchron */
        AudioQueueDispose(g_queue, true);
        g_queue   = NULL;
        g_running = 0;
    }

    fprintf(stderr, "leuae-audio: geschlossen\n");
}
