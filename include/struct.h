#ifndef STRUCT_H
#define STRUCT_H

#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef struct s_mpu_mesures
{
    float acceleration_x;
    float acceleration_y;
    float acceleration_z;

    float gyro_x;
    float gyro_y;
    float gyro_z;

    float temperature;
} mpu_mesures_t;

typedef struct s_calibration_mpu
{
    float gyro_x;
    float gyro_y;
    float gyro_z;
} calibration_mpu_t;

typedef struct s_mpu_state
{
    bool connection;
    bool calibration_valide;
    volatile bool calibration_requested;

    uint8_t identite;

    TaskHandle_t task_handle;

    calibration_mpu_t calibration;
    mpu_mesures_t mesures;
} mpu_state_t;

typedef struct s_button_state
{
    gpio_num_t pin;
    volatile bool pressed;
} button_state_t;

typedef struct s_led_state
{
    gpio_num_t pin;
    bool led_state;
} led_state_t;

typedef struct s_drone
{
    mpu_state_t mpu;
    button_state_t button;
    led_state_t led;
} drone_t;

#endif