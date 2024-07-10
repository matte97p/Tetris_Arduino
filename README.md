# Tetris Arduino

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Tetris_Arduino is an implementation of the classic Tetris game for Arduino with an OLED display. This project showcases a small menu system and the ability to play Tetris directly from an Arduino device.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

## Introduction
This project implements a version of Tetris for the Arduino platform, utilizing an OLED display for graphics. It includes a simple main menu for game selection and offers a nostalgic gaming experience on a microcontroller.

## Features
- Classic Tetris gameplay
- Simple main menu with game selection
- Display icons and game titles
- Smooth gameplay with responsive controls

## Hardware Requirements
To run this project, you will need the following hardware:
- Arduino board (e.g., Arduino Uno, Mega, or similar)
- OLED display (128x64)
- Push buttons for game control
- Connecting wires and breadboard

## Software Requirements
Ensure you have the following software installed:
- [Arduino IDE](https://www.arduino.cc/en/software)
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit SSD1306 Library](https://github.com/adafruit/Adafruit_SSD1306)

## Installation
1. **Clone the repository:**
   ```sh
   git clone https://github.com/matte97p/Tetris_Arduino.git
   cd Tetris_Arduino
   ```

2. **Open the project in Arduino IDE:**
- Launch Arduino IDE.
- Navigate to `File` -> `Open...` and select `Tetris_Optimazed` from the cloned repository.

3. **Upload the code:**
- Connect your Arduino board to your computer.
- Select the correct board and port from the Tools menu.
- Click on the upload button to upload the code to your Arduino.

## Usage
1. **Connect the OLED display and buttons:**
- Connect the OLED display to the Arduino as per the pin configuration mentioned in the code.
- Connect the buttons for game control (e.g., up, down, left, right, select).

2. Power up your Arduino:
- Once the code is uploaded and the hardware is connected, power up your Arduino board.
- The main menu should be displayed on the OLED screen.

3. Navigate the menu and play:
- Use the buttons to navigate the menu and select Tetris to start playing.
- Enjoy the game!

## Contributing
Contributions are welcome! If you have any improvements or bug fixes, feel free to open a pull request or submit an issue. Please ensure that your contributions align with the overall style and aim of the project.

## License
This project is licensed under the MIT License - see the [LICENSE](license.md) file for details.

## Acknowledgments
- Inspired by the classic Tetris game.

Thank you for checking out Tetris_Arduino! If you have any questions or need further assistance, feel free to open an issue or contact the project maintainer.
