#include <led.h>
#include <struct.h>

// esp_err_t led_configuration(led_state_t *led)
// {
//     if (led == NULL)
//         return ESP_ERR_INVALID_ARG;

//     led->pin = GPIO_NUM_8;
//     led->led_state = false;

//     gpio_config_t config = {
//         .pin_bit_mask = 1ULL << led->pin,
//         .mode = GPIO_MODE_OUTPUT,
//         .pull_up_en = GPIO_PULLUP_DISABLE,
//         .pull_down_en = GPIO_PULLDOWN_DISABLE,
//         .intr_type = GPIO_INTR_DISABLE
//     };

//     esp_err_t result = gpio_config(&config);

//     if (result != ESP_OK)
//         return result;

//     return gpio_set_level(
//         led->pin,
//         1
//     );
// }

void	init_led(int gpio_num) {
	gpio_config_t io_conf = {
		.pin_bit_mask = (1ULL << gpio_num),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE
	};
	ESP_ERROR_CHECK(gpio_config(&io_conf));
}

void	update_led(int gpio_num, led_state_t state) {
	if (state != LED_ON && state != LED_OFF) {
		ESP_LOGW("LED", "Invalid LED state: %d", state);
		return;
	}
	ESP_ERROR_CHECK(gpio_set_level(gpio_num, state == LED_ON ? GPIO_LED_ON : GPIO_LED_OFF));
	ESP_LOGI("LED", "LED state updated: GPIO=%d, State=%s", gpio_num, state == LED_ON ? "ON" : "OFF");
}

led_state_t	get_led_state(int gpio_num) {
	int gpio_value = gpio_get_level(gpio_num);
	return (gpio_value == GPIO_LED_ON) ? LED_ON : LED_OFF;
}
