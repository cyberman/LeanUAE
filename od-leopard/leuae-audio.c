/*
 * LeUAE - Leopard UAE
 * leuae-audio.c — Audio via CoreAudio AudioQueue
 * Ersetzt: od-fs/fsemu/fsemu-sdlaudio.c
 * STATUS: STUB
 */

#include <stdint.h>
#include <stdio.h>

/* TODO: #include <CoreAudio/CoreAudio.h> */
/* TODO: #include <AudioUnit/AudioUnit.h> */

int leuae_audio_init(int sample_rate, int channels, int buffer_frames)
{
    /* TODO: AudioQueueNewOutput */
    fprintf(stderr, "leuae-audio: STUB init %dHz %dch\n", sample_rate, channels);
    return 0;
}

void leuae_audio_write(const int16_t *samples, int num_frames)
{
    /* TODO: AudioQueueEnqueueBuffer */
    (void)samples;
    (void)num_frames;
}

void leuae_audio_close(void)
{
    /* TODO: AudioQueueDispose */
}
