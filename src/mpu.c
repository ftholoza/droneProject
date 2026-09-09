#include "mpu.h"

#include <stdint.h>

#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "calibration.h"
#include "struct.h"


static const char *TAG = "MPU6050";
static i2c_master_bus_handle_t bus_i2c;
static i2c_master_dev_handle_t mpu_device;

//configuration du bus
static esp_err_t configurer_i2c(void)
{
    i2c_master_bus_config_t configuration_bus = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true
    };

    esp_err_t resultat = i2c_new_master_bus(
        &configuration_bus,
        &bus_i2c
    );

    if (resultat != ESP_OK) {
        return resultat;
    }

    i2c_device_config_t configuration_mpu = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = MPU_ADRESSE,
        .scl_speed_hz = I2C_FREQUENCE
    };

    return i2c_master_bus_add_device(
        bus_i2c,
        &configuration_mpu,
        &mpu_device
    );
}


//ecrire dans un registre 
static esp_err_t ecrire_registre(
    uint8_t registre,
    uint8_t valeur
)
{
    uint8_t message[2] = {
        registre,
        valeur
    };

    return i2c_master_transmit(
        mpu_device,
        message,
        sizeof(message),
        100
    );
}

static esp_err_t lire_plusieurs_registres(
    uint8_t premier_registre,
    uint8_t *destination,
    size_t nombre_octets
)
{
    return i2c_master_transmit_receive(
        mpu_device,
        &premier_registre,
        1,
        destination,
        nombre_octets,
        100
    );
}

static esp_err_t lire_registre(
    uint8_t registre,
    uint8_t *valeur
)
{
    return lire_plusieurs_registres(
        registre,
        valeur,
        1
    );
}


esp_err_t mpu_init(mpu_state_t *mpu)
{
    if (mpu == NULL)
        return ESP_ERR_INVALID_ARG;

    /*
     * Etat initial.
     */
    mpu->connection = false;
    mpu->calibration_valide = false;
    mpu->calibration_requested = false;
    mpu->identite = 0;
    mpu->task_handle = NULL;
    memset(
        &mpu->calibration,
        0,
        sizeof(mpu->calibration)
    );

    esp_err_t resultat = configurer_i2c();

    if (resultat != ESP_OK)
    {
        ESP_LOGE(TAG, "Impossible de configurer I2C");
        return resultat;
    }

    /*
     * Lecture de l'identifiant WHO_AM_I.
     */
    resultat = lire_registre(
        MPU_REGISTRE_IDENTITE,
        &mpu->identite
    );

    if (resultat != ESP_OK)
    {
        ESP_LOGE(TAG, "Le MPU6050 ne repond pas");
        return resultat;
    }

    ESP_LOGI(
        TAG,
        "Identite recue : 0x%02X",
        mpu->identite
    );

    if (mpu->identite != 0x68)
    {
        ESP_LOGE(
            TAG,
            "Le composant detecte n'est pas un MPU6050"
        );

        return ESP_ERR_INVALID_RESPONSE;
    }

    /*
        PWR_MGMT_1
        Bit :    7       6       5      4     3       2 1 0
        Role : RESET   SLEEP   CYCLE    -   TEMP_DIS   CLKSEL
     */
    resultat = ecrire_registre(
        MPU_REGISTRE_ENERGIE,
        0x01
    );

    if (resultat != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "Impossible de reveiller le MPU6050"
        );

        return resultat;
    }

    /* DLPF active. Bande passante gyro ≈ 98 Hz. */
    resultat = ecrire_registre(
        0x1A,
        0x02
    );

    if (resultat != ESP_OK)
        return resultat;

    /* Frequence : 1000 / (1 + 0) = 1000 Hz */
    resultat = ecrire_registre(
        0x19,
        0x00
    );

    if (resultat != ESP_OK)
        return resultat;

    mpu->connection = true;

    ESP_LOGI(
        TAG,
        "MPU6050 detecte et initialise"
    );

    return ESP_OK;
}


//check si mpu est connecté
bool mpu_est_connecte(void)
{
    uint8_t identite = 0;

    esp_err_t resultat = lire_registre(
        MPU_REGISTRE_IDENTITE,
        &identite
    );

    return resultat == ESP_OK && identite == 0x68;
}

//le bus i2c transporte 1 octet et les mesures sont sur 2 octets 16bits
static int16_t convertir_16_bits(
    uint8_t octet_haut,
    uint8_t octet_bas
)
{
    return (int16_t)(
        ((uint16_t)octet_haut << 8) |
        octet_bas
    );
}

esp_err_t mpu_lire_mesures(
    mpu_mesures_t *mesures
)
{
    if (mesures == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t donnees[14];

    esp_err_t resultat = lire_plusieurs_registres(
        MPU_REGISTRE_MESURES,
        donnees,
        sizeof(donnees)
    );

    if (resultat != ESP_OK) {
        return resultat;
    }

    int16_t acceleration_x_brute =
        convertir_16_bits(donnees[0], donnees[1]);

    int16_t acceleration_y_brute =
        convertir_16_bits(donnees[2], donnees[3]);

    int16_t acceleration_z_brute =
        convertir_16_bits(donnees[4], donnees[5]);

    int16_t temperature_brute =
        convertir_16_bits(donnees[6], donnees[7]);

    int16_t gyro_x_brut =
        convertir_16_bits(donnees[8], donnees[9]);

    int16_t gyro_y_brut =
        convertir_16_bits(donnees[10], donnees[11]);

    int16_t gyro_z_brut =
        convertir_16_bits(donnees[12], donnees[13]);

    /*
     * Réglages par défaut :
     * accélération : -2 à +2 g, de -32768 à +32767 donc 16384 unités par g
     * gyroscope : de -250 degres/s à + 250 degrés/s, donc 131 unités par degré/s
     * formule temperature fournie par la doc 
     */
    mesures->acceleration_x =
        acceleration_x_brute / 16384.0f;

    mesures->acceleration_y =
        acceleration_y_brute / 16384.0f;

    mesures->acceleration_z =
        acceleration_z_brute / 16384.0f;

    mesures->gyro_x =
        gyro_x_brut / 131.0f;

    mesures->gyro_y =
        gyro_y_brut / 131.0f;

    mesures->gyro_z =
        gyro_z_brut / 131.0f;

    mesures->temperature =
        temperature_brute / 340.0f + 36.53f;

    return ESP_OK;
}

void tache_mpu(void *arg)
{
    drone_t *drone = (drone_t *)arg;

    uint32_t compteur_affichage = 0;
    uint32_t compteur_led = 0;

    while (true) {
        /*
         * Attend la notification envoyée par
         * l’interruption du timer.
         */
        ulTaskNotifyTake(
            pdTRUE,
            portMAX_DELAY
        );

        /*
         * Une demande de calibration a été produite
         * par le bouton BOOT.
         */
        if (drone->mpu.calibration_requested) {
            /*
             * Désactive temporairement l’interruption
             * du bouton pour éviter les rebonds.
             */
            gpio_intr_disable(drone->button.pin);
            drone->mpu.calibration_requested = false;

            drone->mpu.calibration_valide = false;
            drone->led.led_state = false;
            gpio_set_level(drone->led.pin, 1);

            printf(
                "Calibration : ne bougez pas le MPU6050\n"
            );

            esp_err_t resultat_calibration =
                calibrer_gyroscope(
                    &drone->mpu.calibration,
                    2000
                );

            if (resultat_calibration == ESP_OK) {
                drone->mpu.calibration_valide = true;

                printf(
                    "Calibration terminee\n"
                );
            }
            else {
                printf(
                    "Calibration echouee : %s\n",
                    esp_err_to_name(
                        resultat_calibration
                    )
                );
            }
            /*Anti rebonds*/
            vTaskDelay(
                pdMS_TO_TICKS(50)
            );

            compteur_affichage = 0;
            compteur_led = 0;

            gpio_intr_enable(drone->button.pin);
        }

        esp_err_t resultat =
            mpu_lire_mesures(&drone->mpu.mesures);

        if (resultat == ESP_OK) {
            drone->mpu.connection = true;

            /*
             * N’applique les biais que si une
             * calibration a réellement réussi.
             */
            if (drone->mpu.calibration_valide) {
                appliquer_calibration(
                    &drone->mpu.calibration,
                    &drone->mpu.mesures
                );
            }

            /*
             * LED : changement toutes les
             * 500 lectures réussies.
             */
            compteur_led++;

            if (compteur_led >= 500) {
                drone->led.led_state = !drone->led.led_state;

                gpio_set_level(
                    drone->led.pin,
                    !drone->led.led_state
                );

                compteur_led = 0;
            }

            /*
             * Affichage toutes les 100 lectures.
             */
            compteur_affichage++;

            if (compteur_affichage >= 200) {
                printf(
                    "Calibration : %s\n",
                    drone->mpu.calibration_valide
                        ? "oui"
                        : "non"
                );

                printf(
                    "Accel : X=%6.2f g Y=%6.2f g Z=%6.2f g\n",
                    drone->mpu.mesures.acceleration_x,
                    drone->mpu.mesures.acceleration_y,
                    drone->mpu.mesures.acceleration_z
                );

                printf(
                    "Gyro : X=%6.2f deg/s Y=%6.2f deg/s Z=%6.2f deg/s\n",
                    drone->mpu.mesures.gyro_x,
                    drone->mpu.mesures.gyro_y,
                    drone->mpu.mesures.gyro_z
                );

                printf(
                    "Temperature : %.2f C\n\n",
                    drone->mpu.mesures.temperature
                );

                compteur_affichage = 0;
            }
        }
        else {
            drone->mpu.connection = false;
            drone->led.led_state = false;

            compteur_led = 0;
            compteur_affichage = 0;

            gpio_set_level(drone->led.pin, 1);
        }
    }
}