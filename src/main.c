
#include "host/app/host_app.h"

int main(int argc, char **argv)
{
    if (host_app_init(argc, argv) != 0) {
        return 1;
    }

    host_app_run();
    host_app_shutdown();

    return 0;
}

