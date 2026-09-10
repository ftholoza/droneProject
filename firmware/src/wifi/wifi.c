#include <wifi.h>

#define WIFI_SSID	"ESP32C3_TEST"
#define WIFI_PASS	"esp12345"
#define MAX_STA_CONN	2

static const char *TAG = "WIFI_AP";

void	wifi_init_softap(void) {
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	esp_netif_create_default_wifi_ap();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	wifi_config_t wifi_config = {
		.ap = {
			.ssid = WIFI_SSID,
			.ssid_len = strlen(WIFI_SSID),
			.channel = 1,
			.password = WIFI_PASS,
			.max_connection = MAX_STA_CONN,
			.authmode = WIFI_AUTH_WPA_WPA2_PSK
		},
	};

	if (strlen(WIFI_PASS) == 0) {
		wifi_config.ap.authmode = WIFI_AUTH_OPEN;
	}

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "WiFi AP started, SSID: %s | Password: %s", WIFI_SSID, WIFI_PASS);
	ESP_LOGI(TAG, "Connect to the AP and check the IP address assigned to the ESP32C3.");
}
