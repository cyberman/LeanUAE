/*
 * LeUAE - Leopard UAE
 * leuae-input.c — Keyboard/Mouse via Carbon Event Manager
 * Ersetzt: od-fs/fsemu/fsemu-sdlinput.c
 * STATUS: STUB
 */

#include <stdint.h>
#include <stdio.h>

/* TODO: #include <Carbon/Carbon.h> */

int leuae_input_init(void)
{
    /* TODO: InstallEventHandler kEventClassKeyboard/Mouse */
    fprintf(stderr, "leuae-input: STUB init\n");
    return 0;
}

int leuae_input_poll(void)
{
    /* TODO: ReceiveNextEvent → Amiga-Keycodes */
    return 1;
}

void leuae_input_close(void)
{
    /* TODO: RemoveEventHandler */
}
