const express = require('express');
const axios = require('axios');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 4000;
const ESP32_IP = process.env.ESP32_IP || 'http://YOUR_ESP32_IP';

app.use(express.static(path.join(__dirname, 'public')));

app.get('/servo', async (req, res) => {
  const { id, angle } = req.query;

  try {
    await axios.get(`${ESP32_IP}/servo?id=${id}&angle=${angle}`);
    console.log(`[${new Date().toLocaleTimeString()}] Servo ${id} → ${angle}°`);
    res.send('Command sent');
  } catch (err) {
    console.error(`[${new Date().toLocaleTimeString()}] Failed: ${err.message}`);
    res.status(500).send('Failed to send command');
  }
});

app.get('/get-emg', async (req, res) => {
  try {
    const response = await axios.get(`${ESP32_IP}/emg`);
    res.json(response.data);
  } catch (err) {
    console.error(`[${new Date().toLocaleTimeString()}] EMG fetch failed: ${err.message}`);
    res.status(500).json({ error: 'Failed to fetch EMG data' });
  }
});

app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});
