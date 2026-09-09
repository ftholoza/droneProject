#include "led.h"
#include "struct.h"

esp_err_t led_configuration(led_state_t *led)
{
    if (led == NULL)
        return ESP_ERR_INVALID_ARG;

    led->pin = GPIO_NUM_8;
    led->led_state = false;

    gpio_config_t config = {
        .pin_bit_mask = 1ULL << led->pin,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    esp_err_t result = gpio_config(&config);

    if (result != ESP_OK)
        return result;

    return gpio_set_level(
        led->pin,
        1
    );
}