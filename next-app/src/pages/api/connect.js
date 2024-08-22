import noble from 'noble';

export default function handler(req, res) {
  const { address } = req.body;
  
  noble.on('stateChange', (state) => {
    if (state === 'poweredOn') {
      noble.startScanning();
    } else {
      noble.stopScanning();
    }
  });

  noble.on('discover', (peripheral) => {
    if (peripheral.address === address) {
      noble.stopScanning();
      peripheral.connect((error) => {
        if (error) {
          res.status(500).json({ error: error.message });
          return;
        }
        res.status(200).json({ message: 'Connected' });
      });
    }
  });
}
