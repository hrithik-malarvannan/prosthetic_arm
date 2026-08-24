# EMG-Controlled Prosthetic Hand

An ESP32-based prosthetic hand controlled using muscle activity captured through an EMG sensor. The system uses five servo motors to actuate the fingers and includes a web-based dashboard for manual servo control and EMG monitoring.

## Features

- EMG-based muscle activity detection
- Five-finger servo control
- ESP32-based control system
- EMG signal smoothing and filtering
- Adaptive EMG baseline calibration
- Automatic finger movement based on muscle activity
- Manual servo control through a web dashboard
- EMG value monitoring
- Individual finger positioning
- All-finger positioning

## System Architecture

    EMG Sensor
         |
         v
       ESP32
         |
    +----+----+
    |         |
    v         v
EMG Signal  Servo Control
Processing       |
                |
       +--------+--------+
       |        |        |
     Thumb    Index    Middle
       |        |        |
      Ring     Pinky

         ESP32
           |
           v
     Node.js Server
           |
           v
     Web Dashboard

## Hardware

- ESP32
- EMG sensor
- 5 x Servo motors
- Prosthetic hand mechanism
- External power supply
- Connecting wires

## Software

- Arduino IDE
- ESP32 Arduino Core
- ESP32Servo library
- Node.js
- Express.js
- Axios
- HTML
- CSS
- JavaScript

## Pin Configuration

| Component | ESP32 Pin |
|-----------|-----------|
| EMG Sensor | GPIO 36 |
| Thumb Servo | GPIO 12 |
| Index Servo | GPIO 14 |
| Middle Servo | GPIO 27 |
| Ring Servo | GPIO 26 |
| Pinky Servo | GPIO 25 |

## Project Structure

    prosthetic-arm/
    |
    +-- .gitignore
    +-- README.md
    |
    +-- esp32/
    |   +-- prosthetic_arm.ino
    |
    +-- web-dashboard/
        +-- server.js
        +-- index.html
        +-- style.css

## ESP32 Setup

1. Install Arduino IDE.
2. Install ESP32 board support in Arduino IDE.
3. Install the ESP32Servo library.
4. Open esp32/prosthetic_arm.ino.
5. Select the appropriate ESP32 board.
6. Select the correct COM port.
7. Connect the EMG sensor and servo motors according to the pin configuration.
8. Upload the code to the ESP32.
9. Open the Serial Monitor at 115200 baud.
10. Keep the forearm relaxed during the initial EMG calibration.
11. Flex the muscle to control the prosthetic hand.

## Web Dashboard Setup

Navigate to the dashboard directory:

    cd web-dashboard

Install the required dependencies:

    npm install express axios

Set the ESP32 IP address as an environment variable.

### Windows Command Prompt

    set ESP32_IP=http://YOUR_ESP32_IP
    node server.js

### Windows PowerShell

    $env:ESP32_IP="http://YOUR_ESP32_IP"
    node server.js

The dashboard server runs on:

    http://localhost:4000

Open the above address in a web browser after starting the server.

## EMG Processing

The ESP32 processes the EMG signal through multiple stages:

1. Raw EMG signal acquisition
2. Exponential smoothing
3. Moving-average filtering
4. Baseline calibration
5. Adaptive baseline adjustment
6. Muscle activity calculation
7. Activity-to-servo position mapping
8. Servo movement smoothing

The detected muscle activity is mapped to a servo position between 0 and 180 degrees.

## Web Dashboard

The web dashboard provides:

- EMG value monitoring
- Hand state monitoring
- Individual finger servo control
- All-finger control
- Servo position adjustment from 0 to 180 degrees

The dashboard communicates with the ESP32 through a Node.js and Express server.

## Servo Control

Each finger is controlled using an individual servo motor.

The dashboard provides three predefined positions:

- 0 degrees - Open
- 90 degrees - Intermediate position
- 180 degrees - Closed

Individual finger sliders can also be used for manual positioning.

## Calibration

When the ESP32 starts, it performs an initial EMG baseline calibration.

During calibration:

- Keep the forearm relaxed.
- Avoid unnecessary movement.
- Allow the system to measure the resting EMG signal.

The calculated baseline is then used to determine muscle activity.

## Applications

This project can serve as a prototype platform for:

- EMG-based prosthetic systems
- Assistive robotic devices
- Human-machine interfaces
- Biomedical engineering prototypes
- Gesture-controlled robotic systems
- Educational embedded systems projects

## Future Improvements

Possible improvements include:

- Multiple EMG channels for individual finger control
- Wireless communication between the ESP32 and dashboard
- Improved EMG signal conditioning
- Machine-learning-based gesture classification
- More natural finger movement
- Force and grip sensing
- Battery-powered operation
- 3D-printed prosthetic structure
- Mobile application integration

## Disclaimer

This project is an educational and research prototype and is not intended for direct medical use.

## License

This project is intended for educational and prototyping purposes.
