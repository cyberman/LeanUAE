#ifndef LEUAE_HOST_TIME_H
#define LEUAE_HOST_TIME_H

#include <stdint.h>

uint64_t host_time_now_us(void);
void host_time_sleep_us(uint64_t usec);
void host_time_sleep_ms(unsigned int msec);

#endif

