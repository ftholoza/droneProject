#include "button.h"

esp_err_t configure_button(button_state_t *button)
{
    if (button == NULL)
        return ESP_ERR_INVALID_ARG;

    button->pin = GPIO_NUM_9;
    button->pressed = false;

    gpio_config_t config = {
        .pin_bit_mask = 1ULL << button->pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };

    return gpio_config(&config);
}