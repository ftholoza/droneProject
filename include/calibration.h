#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <stdint.h>

#include "esp_err.h"
#include "struct.h"

esp_err_t calibrer_gyroscope(
    calibration_mpu_t *calibration,
    uint32_t nombre_mesures
);

void appliquer_calibration(
    const calibration_mpu_t *calibration,
    mpu_mesures_t *mesures
);

#endif