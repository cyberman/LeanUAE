#ifndef LEUAE_TARGET_H
#define LEUAE_TARGET_H

/*
 * LeUAE - Leopard UAE
 * Platform target definitions for macOS 10.5.8 / PowerPC
 */

#define TARGET_NAME "leuae"
#define NO_MAIN_IN_MAIN_C
#define OPTIONSFILENAME "default.uae"

uint8_t *uae_get_render_buffer(void);

extern int g_uae_deterministic_mode;
static inline int uae_deterministic_mode(void) {
    return g_uae_deterministic_mode;
}
void uae_deterministic_amiga_time(int *days, int *mins, int *ticks);
void filesys_host_init(void);

#endif /* LEUAE_TARGET_H */
