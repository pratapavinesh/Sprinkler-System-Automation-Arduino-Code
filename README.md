# Automation of Sprinkler System with Arduino

## Overview

This project integrates an Arduino-based system to automate a sprinkler system for open cast coal mines roads. The system measures moisture level, dust level, and vehicle movement to determine whether the sprinkler valve should be opened or closed. The collected data is sent to Firebase Realtime Database for real-time monitoring and analysis.

## Arduino Code

The Arduino code provided here measures the sensor values and controls the sprinkler valve based on the following criteria:

- **Moisture Level**: Determines soil moisture to decide whether irrigation is required.
- **Dust Level**: Monitors dust concentration in the air to trigger sprinkler activation when levels exceed a threshold.
- **Vehicle Movement**: Detects vehicle presence to activate sprinklers for dust suppression.

### Installation

1. **Setup Arduino IDE**: Install the Arduino IDE from [arduino.cc](https://www.arduino.cc/en/Main/Software).
2. **Upload Code**: Open the `dustSuppressionAutomationCode.ino` file in Arduino IDE and upload it to your Arduino board.

### Components Used

- Arduino Uno or compatible board
- Moisture sensor
- Dust sensor
- Motion sensor (for vehicle movement detection)
- Relay module (for controlling sprinkler valve)

### Usage

1. **Run Arduino Code**: Upload the `dustSuppressionAutomationCode.ino` to your Arduino board.
2. **Monitor Data**: View real-time data on Firebase Realtime Database to monitor moisture level, dust level, and vehicle movement.

## Firebase Realtime Database Integration

The Arduino sends sensor data (moisture level, dust level, vehicle movement) to Firebase Realtime Database for remote monitoring and analysis. Ensure your Firebase credentials are correctly configured in the Arduino code (`firebase_config.h`).

### Firebase Configuration

1. **Create Firebase Project**: Set up a Firebase project at [console.firebase.google.com](https://console.firebase.google.com).
2. **Generate Credentials**: Obtain Firebase Admin SDK credentials and configure them in `firebase_config.h`.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with improvements.

## License

Distributed under the MIT License. See `LICENSE` for more information.
