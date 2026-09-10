import React, { useState } from 'react';

import { UdpService } from './services/udp';

export default function App() {
	const [ledState, setLedState] = useState(false);
	const udpService = new UdpService();

	const toggleLed = () => {
		const nextState = !ledState;
		setLedState(nextState);
		udpService.updateLedState(nextState)
	};

	return (
		<div style={styles.container}>
		<h2>🛸 Dashboard Drone ESP32 (UDP)</h2>

		<div style={styles.card}>
			<p>Contrôle Rapide (UDP Latence &lt; 2ms)</p>
			<button
			onClick={toggleLed}
			style={{
				...styles.button,
				backgroundColor: ledState ? '#4caf50' : '#333'
			}}
			>
			LED : {ledState ? 'ALLUMÉE' : 'ÉTEINTE'}
			</button>
		</div>
		</div>
	);
}

const styles = {
  container: { backgroundColor: '#121212', color: '#fff', minHeight: '100vh', padding: '20px', fontFamily: 'sans-serif' },
  card: { backgroundColor: '#1e1e1e', padding: '20px', borderRadius: '10px', maxWidth: '400px' },
  button: { padding: '15px 30px', fontSize: '18px', color: '#fff', border: 'none', borderRadius: '8px', cursor: 'pointer', marginTop: '10px' }
};
