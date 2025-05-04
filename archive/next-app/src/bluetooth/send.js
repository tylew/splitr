import noble from 'noble';

export default function handler(req, res) {
  const { address, data } = req.body;

  noble.on('discover', (peripheral) => {
    if (peripheral.address === address) {
      peripheral.connect((error) => {
        if (error) {
          res.status(500).json({ error: error.message });
          return;
        }

        const serviceUUIDs = []; // specify service UUIDs here
        const characteristicUUIDs = []; // specify characteristic UUIDs here

        peripheral.discoverSomeServicesAndCharacteristics(
          serviceUUIDs,
          characteristicUUIDs,
          (error, services, characteristics) => {
            if (error) {
              res.status(500).json({ error: error.message });
              return;
            }

            const characteristic = characteristics[0]; // assuming you have the right characteristic
            characteristic.write(Buffer.from(data), true, (error) => {
              if (error) {
                res.status(500).json({ error: error.message });
                return;
              }
              res.status(200).json({ message: 'Data sent' });
            });
          }
        );
      });
    }
  });

  noble.startScanning();
}
