#include "calibration.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "struct.h"
#include "mpu.h"

#define NOMBRE_DE_MESURES 2000 //frequence du mpu 1Khz donc 1000 = 1sec

esp_err_t calibrer_gyroscope(calibration_mpu_t *calibration,
     uint32_t nombre_mesures)
{
    if (calibration == NULL || nombre_mesures == 0) {
        return (ESP_ERR_INVALID_ARG);
    }

    mpu_mesures_t mesures;
    float somme_x = 0;
    float somme_y = 0;
    float somme_z = 0;

    esp_err_t resultat = mpu_lire_mesures(&mesures);
    if (resultat != ESP_OK)
        return (ESP_ERR_INVALID_RESPONSE);
    vTaskDelay(pdMS_TO_TICKS(100));

    for (int i = 0; i < NOMBRE_DE_MESURES; i++)
    {
        esp_err_t resultat = mpu_lire_mesures(&mesures);
        if (resultat != ESP_OK)
        {
            printf("calibration échouée");
            return (ESP_ERR_INVALID_RESPONSE);
        }
        somme_x += mesures.gyro_x;
        somme_y += mesures.gyro_y;
        somme_z += mesures.gyro_z;
    }
    calibration->gyro_x = somme_x / nombre_mesures;
    calibration->gyro_y = somme_y / nombre_mesures;
    calibration->gyro_z = somme_z / nombre_mesures;
    printf("calibration terminée : X=%.3f Y=%.3f Z=%3f", 
        calibration->gyro_x,
        calibration->gyro_y,
        calibration->gyro_z);
    return(ESP_OK);

}

void appliquer_calibration(
    const calibration_mpu_t *calibration,
    mpu_mesures_t *mesures
)
{
    if (calibration == NULL || mesures == NULL) {
        return;
    }

    mesures->gyro_x -= calibration->gyro_x;
    mesures->gyro_y -= calibration->gyro_y;
    mesures->gyro_z -= calibration->gyro_z;
}