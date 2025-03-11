import type { NextApiRequest, NextApiResponse } from 'next';
import { exec } from 'child_process';

type Device = {
  address: string;
  name: string;
};

type Data = Device[];

export default function handler(req: NextApiRequest, res: NextApiResponse<Data | { error: string }>) {
  exec('hcitool scan', (error, stdout, stderr) => {
    if (error) {
      res.status(500).json({ error: error.message });
      return;
    }
    const devices = stdout
      .split('\n')
      .slice(1)
      .map(line => {
        const [address, name] = line.trim().split('\t');
        return { address, name };
      })
      .filter(device => device.address);
    res.status(200).json(devices);
  });
}
