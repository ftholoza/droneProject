#pragma once

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include <lwip/sockets.h>

#include <stdint.h>

typedef enum __attribute__((packed)) {
	MSG_TYPE_LED = 0x01,
}	msg_type_t;

typedef enum __attribute__((packed)) {
	GET = 0x01,
	SET = 0x02,
}	method_t;

typedef enum __attribute__((packed)) {
	LED_OFF = 0x00,
	LED_ON = 0x01,
}	led_state_t;

typedef struct __attribute__((packed)) {
	msg_type_t	type;
	method_t	method;
	led_state_t	state;
}	led_cmd_t;

typedef struct __attribute__((packed)) {
	msg_type_t	type;
	led_state_t	state;
}	led_resp_t;

void	wifi_init_softap(void);
void	udp_server_task(void *pvParameters);
