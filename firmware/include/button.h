#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

#include "driver/gpio.h"
#include "esp_err.h"
#include "struct.h"

esp_err_t configure_button(button_state_t *button);

#endif