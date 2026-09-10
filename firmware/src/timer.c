#include "timer.h"

#include "esp_err.h"

static gptimer_handle_t timer;

void timer_configuration(
    gptimer_alarm_cb_t callback,
    void *user_data,
    uint64_t periode_microsecondes
)
{
    gptimer_config_t configuration = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000
    };

    ESP_ERROR_CHECK(
        gptimer_new_timer(
            &configuration,
            &timer
        )
    );

    gptimer_event_callbacks_t callbacks = {
        .on_alarm = callback
    };

    ESP_ERROR_CHECK(
        gptimer_register_event_callbacks(
            timer,
            &callbacks,
            user_data
        )
    );

    gptimer_alarm_config_t alarme = {
        .alarm_count = periode_microsecondes,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true
    };

    ESP_ERROR_CHECK(
        gptimer_set_alarm_action(
            timer,
            &alarme
        )
    );

    ESP_ERROR_CHECK(
        gptimer_enable(timer)
    );

    ESP_ERROR_CHECK(
        gptimer_start(timer)
    );
}