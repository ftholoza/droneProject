#ifndef INTERRUPTIONS_H
#define INTERRUPTIONS_H

#include "driver/gptimer.h"
#include "struct.h"

bool interruption_mpu(
    gptimer_handle_t timer,
    const gptimer_alarm_event_data_t *event_data,
    void *user_data
);

esp_err_t configure_interruptions(drone_t *drone);

#endif