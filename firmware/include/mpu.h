#ifndef MPU_H
#define MPU_H

#include "esp_err.h"
#include "struct.h"

#define I2C_FREQUENCE 400000
#define SDA_PIN GPIO_NUM_4
#define SCL_PIN GPIO_NUM_5

#define MPU_ADRESSE            0x68
#define MPU_REGISTRE_IDENTITE  0x75
#define MPU_REGISTRE_ENERGIE   0x6B
#define MPU_REGISTRE_MESURES   0x3B

esp_err_t mpu_init(mpu_state_t *mpu);

bool mpu_est_connecte(void);

esp_err_t mpu_lire_mesures(
    mpu_mesures_t *mesures
);

void tache_mpu(void *arg);

#endif