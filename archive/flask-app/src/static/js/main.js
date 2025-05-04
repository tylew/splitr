document.getElementById('scan').addEventListener('click', function() {
    const loadingIndicator = document.getElementById('loading');
    const devicesList = document.getElementById('devices');
    const scanBtn = document.getElementById('scan');

    // Show the loading indicator
    loadingIndicator.style.display = 'block';
    scanBtn.style.display = 'none';
    
    // Clear the devices list
    devicesList.innerHTML = '';

    // Load the list of connected devices first
    fetch('/connections')
        .then(response => response.json())
        .then(connectedDevices => {
            // Perform the scan
            return fetch('/scan')
                .then(response => response.json())
                .then(data => {
                    // Filter out already connected devices
                    const filteredDevices = data.filter(device => {
                        return !connectedDevices.some(conn => conn.address === device.address);
                    });

                    // Display the filtered list of devices
                    filteredDevices.forEach(device => {
                        const li = document.createElement('li');
                        li.textContent = `${device.name} (${device.address})`;
                        const button = document.createElement('button');
                        button.textContent = 'Connect';
                        button.addEventListener('click', function() {
                            connectDevice(device.address);
                            devicesList.removeChild(li);
                        });
                        li.appendChild(button);
                        devicesList.appendChild(li);
                    });
                });
        })
        .catch(error => {
            console.error('Error scanning for devices:', error);
            devicesList.innerHTML = '<li>Error scanning for devices</li>';
        })
        .finally(() => {
            loadingIndicator.style.display = 'none';
            scanBtn.style.display = 'block';
        });
});

function connectDevice(address) {
    fetch('/connect', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ address: address })
    })
    .then(response => response.json())
    .then(data => {
        alert(data.message || data.error);
        loadConnections();
    });
}

function loadConnections() {
    fetch('/connections')
        .then(response => response.json())
        .then(data => {
            const connectionsList = document.getElementById('connections');
            connectionsList.innerHTML = '';
            data.forEach(connection => {
                const li = document.createElement('li');
                li.textContent = `${connection.name} (${connection.address})`;
                connectionsList.appendChild(li);
            });
        });
}

// Load connections on page load
loadConnections();
