import { useState } from 'react';

export default function Dashboard() {
  const [scanResults, setScanResults] = useState([]);
  const [message, setMessage] = useState('');

  const handleScan = async () => {
    try {
      const response = await fetch('/api/scan');
      const data = await response.json();
      setScanResults(data);
      setMessage('Scan completed.');
    } catch (error) {
      setMessage('Error scanning for devices.');
    }
  };

  const handleConnect = async (address) => {
    try {
      const response = await fetch('/api/connect', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ address }),
      });
      const data = await response.json();
      setMessage(data.message);
    } catch (error) {
      setMessage('Error connecting to device.');
    }
  };

  const handleDisconnect = async (address) => {
    try {
      const response = await fetch('/api/disconnect', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ address }),
      });
      const data = await response.json();
      setMessage(data.message);
    } catch (error) {
      setMessage('Error disconnecting from device.');
    }
  };

  const handleSendData = async (address, dataToSend) => {
    try {
      const response = await fetch('/api/send', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ address, data: dataToSend }),
      });
      const data = await response.json();
      setMessage(data.message);
    } catch (error) {
      setMessage('Error sending data to device.');
    }
  };

  return (
    <div style={{ padding: '20px' }}>
      <h1>Bluetooth Control Dashboard</h1>
      
      <button onClick={handleScan}>Scan for Devices</button>
      
      <h2>Scan Results:</h2>
      <ul>
        {scanResults.map((device) => (
          <li key={device.address}>
            {device.name} ({device.address})
            <button onClick={() => handleConnect(device.address)}>Connect</button>
            <button onClick={() => handleDisconnect(device.address)}>Disconnect</button>
            <button onClick={() => handleSendData(device.address, 'Test Data')}>Send Data</button>
          </li>
        ))}
      </ul>
      
      {message && <p>{message}</p>}
    </div>
  );
}
