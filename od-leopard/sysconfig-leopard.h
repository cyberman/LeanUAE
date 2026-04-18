#ifndef LEUAE_SYSCONFIG_H
#define LEUAE_SYSCONFIG_H

/*
 * LeUAE - Leopard UAE
 * System capabilities for macOS 10.5.8 / PowerPC
 * Nur was Leopard nativ mitbringt. Keine externen Libs.
 */

#define SUPPORT_THREADS
#define MMUEMU
#define FULLMMU
#define ECS_DENISE
#define A_ZIP
#define A_LHA
#define A_LZX
#define A_DMS
#define HAVE_DIRENT_H    1
#define HAVE_GMTIME_R    1
#define HAVE_ISINF       1
#define HAVE_ISNAN       1
#define HAVE_NANOSLEEP   1
#define HAVE_STDLIB_H    1
#define HAVE_SYS_STATVFS_H 1
#define HAVE_SYS_TERMIOS_H 1
#define HAVE_UNISTD_H    1
#define HAVE_USLEEP      1
#define WORDS_BIGENDIAN  1
#undef HAVE_SDL
#undef HAVE_GLIB
#undef HAVE_FREETYPE

#endif /* LEUAE_SYSCONFIG_H */
