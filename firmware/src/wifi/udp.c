#include <wifi.h>
#include <led.h>

#define UDP_PORT 8888

void udp_server_task(void *param) {
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_len = sizeof(client_addr);
	char rx_buffer[1024];

	drone_t *drone = (drone_t *)param;

	// Create UDP socket
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		ESP_LOGE("UDP", "Failed to create socket");
		vTaskDelete(NULL);
		return;
	}

	// Bind the socket to the specified port
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(UDP_PORT);

	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		ESP_LOGE("UDP", "Failed to bind socket");
		close(sockfd);
		vTaskDelete(NULL);
		return;
	}

	ESP_LOGI("UDP", "UDP server listening on port %d", UDP_PORT);

	while (1) {
		// Receive data from client
		int len = recvfrom(sockfd, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&client_addr, &addr_len);
		if (len < 0) {
			ESP_LOGE("UDP", "Failed to receive data");
			continue;
		}

		uint8_t msg_type = rx_buffer[0];

		switch (msg_type) {
			case MSG_TYPE_LED: {
				if (len < sizeof(led_cmd_t)) {
					ESP_LOGW("UDP", "Received LED command with invalid length: %d", len);
					break;
				}

				led_cmd_t *cmd = (led_cmd_t *)rx_buffer;

				if (cmd->method == SET) {
					update_led(GPIO_LED_B, cmd->state);
				} else if (cmd->method == GET) {
					led_state_t led_state = get_led_state(GPIO_LED_B);

					led_resp_t response = {
						.type = MSG_TYPE_LED,
						.state = led_state
					};

					sendto(sockfd, &response, sizeof(response), 0, (struct sockaddr *)&client_addr, addr_len);
				} else {
					ESP_LOGW("UDP", "Received LED command with invalid method: %d", cmd->method);
				}

				break;
			}

			case MSG_TYPE_CALIBRATION: {
				ESP_LOGI("UDP", "Received calibration command");
				drone->mpu.calibration_requested = true;
				break;
			}

			default:
				ESP_LOGW("UDP", "Received unknown message type: 0x%02X", msg_type);
				break;
		}
	}

	close(sockfd);
}
