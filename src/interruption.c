#include "interruptions.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

static void IRAM_ATTR button_interrupt(void *arg)
{
    drone_t *drone = (drone_t *)arg;

    if (drone == NULL)
        return;

    drone->button.pressed = true;
    drone->mpu.calibration_requested = true;
}

bool IRAM_ATTR interruption_mpu(
    gptimer_handle_t timer,
    const gptimer_alarm_event_data_t *event_data,
    void *user_data
)
{
    (void)timer;
    (void)event_data;

    drone_t *drone = (drone_t *)user_data;

    if (
        drone == NULL ||
        drone->mpu.task_handle == NULL
    )
    {
        return false;
    }

    BaseType_t task_woken = pdFALSE;

    vTaskNotifyGiveFromISR(
        drone->mpu.task_handle,
        &task_woken
    );

    return task_woken == pdTRUE;
}

esp_err_t configure_interruptions(drone_t *drone)
{
    if (drone == NULL)
        return ESP_ERR_INVALID_ARG;

    esp_err_t result = gpio_install_isr_service(0);

    if (
        result != ESP_OK &&
        result != ESP_ERR_INVALID_STATE
    )
    {
        return result;
    }

    return gpio_isr_handler_add(
        drone->button.pin,
        button_interrupt,
        drone
    );
}