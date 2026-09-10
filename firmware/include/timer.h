#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#include "driver/gptimer.h"

void timer_configuration(
    gptimer_alarm_cb_t callback,
    void *user_data,
    uint64_t periode_microsecondes
);

#endif