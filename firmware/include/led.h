#ifndef LED_H
# define LED_H

#include "driver/gpio.h"
#include "esp_log.h"
#include <wifi.h>

void		init_led(int gpio_num);
void		update_led(int gpio_num, led_state_t state);
led_state_t	get_led_state(int gpio_num);

#define GPIO_LED_B GPIO_NUM_8
#define GPIO_LED_ON 0
#define GPIO_LED_OFF 1

#endif
