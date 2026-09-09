#ifndef LED_H
# define LED_H
#include <stdbool.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer.h"
#include "mpu.h"
#include "esp_log.h"

esp_err_t led_configuration(led_state_t *led);

#define LED_PIN          GPIO_NUM_8;

#endif