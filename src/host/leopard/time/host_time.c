#include "host_time.h"

#include <sys/time.h>
#include <time.h>

uint64_t host_time_now_us(void)
{
    struct timeval tv;

    gettimeofday(&tv, 0);

    return ((uint64_t)tv.tv_sec * 1000000ULL) + (uint64_t)tv.tv_usec;
}

void host_time_sleep_us(uint64_t usec)
{
    struct timespec ts;

    ts.tv_sec = (time_t)(usec / 1000000ULL);
    ts.tv_nsec = (long)((usec % 1000000ULL) * 1000ULL);

    nanosleep(&ts, 0);
}

void host_time_sleep_ms(unsigned int msec)
{
    host_time_sleep_us((uint64_t)msec * 1000ULL);
}

