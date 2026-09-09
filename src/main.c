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

void app_main(void)
{
    static drone_t drone = {0};

    led_configuration(&drone.led);

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

    timer_configuration(
        interruption_mpu,
        &drone,
        1000
    );
}