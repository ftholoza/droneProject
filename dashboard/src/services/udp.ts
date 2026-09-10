import { bind, send } from '@kuyoonjo/tauri-plugin-udp';

const ESP_IP = '192.168.4.1:8888';
const SOCKET_ID = 'drone-socket';

enum Command {
	LED = 0x01,
	CALIBRATION = 0x02,
}

enum Method {
	GET = 0x01,
	SET = 0x02,
}

enum LedState {
	ON = 0x01,
	OFF = 0x00,
}

export class UdpService {
	constructor() {
		this.initSocket();
	}

	private async initSocket() {
		try {
			await bind(SOCKET_ID, '0.0.0.0:0');
		} catch (e) {
			console.error("Erreur d'initialisation du socket UDP :", e);
		}
	}

	private sendCommand = async (bytes: number[]) => {
		try {
			const payload = new Uint8Array(bytes);
			await send(SOCKET_ID, ESP_IP, payload);
			console.log(`Envoyé via UDP : ${payload}`);
		} catch (e) {
			console.error("Erreur d'envoi UDP :", e);
		}
	}

	public updateLedState = async (state: boolean) => {
		this.sendCommand([Command.LED, Method.SET, state ? LedState.ON : LedState.OFF]);
	}

	public getLedState = async (): Promise<boolean> => {
		this.sendCommand([Command.LED, Method.GET]);
		return false; // Placeholder, replace with actual implementation
	}

	public requestCalibration = async () => {
		this.sendCommand([Command.CALIBRATION]);
	}
}
