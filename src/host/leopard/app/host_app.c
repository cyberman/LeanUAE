#include "host_app.h"
#include "../time/host_time.h"

static int g_quit_requested = 0;

int host_app_init(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    g_quit_requested = 0;
    return 0;
}

void host_app_run(void)
{
    while (!g_quit_requested) {
        host_time_sleep_ms(10);
        g_quit_requested = 1;
    }
}

void host_app_request_quit(void)
{
    g_quit_requested = 1;
}

void host_app_shutdown(void)
{
}

