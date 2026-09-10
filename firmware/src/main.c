#include <stdbool.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "timer.h"
#include "mpu.h"
#include "esp_log.h"
#include "calibration.h"
#include "esp_err.h"
#include "struct.h"
#include "button.h"
#include "interruptions.h"
#include "led.h"

#include "nvs_flash.h"

void app_main(void)
{
    static drone_t drone = {0};

	vTaskDelay(pdMS_TO_TICKS(1000));

	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);

    // led_configuration(&drone.led);
	init_led(GPIO_LED_B);

    if (mpu_init(&drone.mpu) != ESP_OK)
    {
        printf("Error in MPU6050 initialisation\n");
        return;
    }

    if (configure_button(&drone.button) != ESP_OK)
    {
        printf("Error in button configuration\n");
        return;
    }

    if (configure_interruptions(&drone) != ESP_OK)
    {
        printf("Error in interrupt configuration\n");
        return;
    }

    xTaskCreate(
        tache_mpu,
        "tache_mpu",
        4096,
        &drone,
        10,
        &drone.mpu.task_handle
    );

	wifi_init_softap();
	xTaskCreate(
		udp_server_task,
		"udp_server_task",
		4096,
		&drone,
		6,
		NULL
	);

	timer_configuration(
		interruption_mpu,
		&drone,
		1000
	);
}
