import noble from 'noble';

export default function handler(req, res) {
  const { address } = req.body;

  noble.on('discover', (peripheral) => {
    if (peripheral.address === address) {
      peripheral.disconnect((error) => {
        if (error) {
          res.status(500).json({ error: error.message });
          return;
        }
        res.status(200).json({ message: 'Disconnected' });
      });
    }
  });
}
