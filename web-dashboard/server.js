require('dotenv').config();

const express = require('express');
const axios = require('axios');
const path = require('path');

const app = express();

const PORT = process.env.PORT || 4000;
const ESP32_IP = process.env.ESP32_IP;

if (!ESP32_IP) {
  console.error('ESP32_IP is not configured.');
  process.exit(1);
}

app.use(express.json());
app.use(express.static(__dirname));

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'index.html'));
});

app.get('/servo', async (req, res) => {
  const { id, angle } = req.query;

  if (id === undefined || angle === undefined) {
    return res.status(400).json({
      error: 'Missing servo ID or angle'
    });
  }

  try {
    await axios.get(
      `${ESP32_IP}/servo?id=${id}&angle=${angle}`
    );

    res.json({
      success: true,
      id,
      angle
    });

  } catch (err) {
    console.error(
      `Servo command failed: ${err.message}`
    );

    res.status(500).json({
      success: false,
      error: 'Failed to communicate with ESP32'
    });
  }
});

app.get('/get-emg', async (req, res) => {
  try {
    const response =
      await axios.get(`${ESP32_IP}/emg`);

    res.json(response.data);

  } catch (err) {
    console.error(
      `EMG fetch failed: ${err.message}`
    );

    res.status(500).json({
      error: 'Failed to fetch EMG data'
    });
  }
});

app.get('/mode', async (req, res) => {
  const { mode } = req.query;

  if (!mode) {
    return res.status(400).json({
      error: 'Mode is required'
    });
  }

  try {
    const response =
      await axios.get(`${ESP32_IP}/mode?mode=${mode}`);

    res.json({
      success: true,
      message: response.data
    });

  } catch (err) {
    res.status(500).json({
      success: false,
      error: 'Failed to change ESP32 mode'
    });
  }
});

app.get('/status', async (req, res) => {
  try {
    const response =
      await axios.get(`${ESP32_IP}/status`);

    res.json(response.data);

  } catch (err) {
    res.status(500).json({
      wifi: false,
      error: 'ESP32 unavailable'
    });
  }
});

app.listen(PORT, () => {
  console.log(`Dashboard running at http://localhost:${PORT}`);
});
