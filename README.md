# Tetris Arduino

## Introduction
This project implements a version of Tetris for the Arduino platform, utilizing an OLED display for graphics. It includes a simple main menu for game selection and offers a nostalgic gaming experience on a microcontroller.

## Features
- Classic Tetris gameplay
- Simple main menu with game selection
- Display icons and game titles
- Smooth gameplay with responsive controls

## Requirements
### Hardware
- Arduino board (Uno, Mega, or similar) -> **Uno for old, Mega or superior for the Optimazed one** [BUY HERE](https://www.amazon.it/Elegoo-ATmega2560-ATMEGA16U2-Compatibile-Arduino/dp/B071W7WP35/ref=asc_df_B071W7WP35/?tag=googshopit-21&linkCode=df0&hvadid=700813659502&hvpos=&hvnetw=g&hvrand=1402956313917872359&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=1008782&hvtargid=pla-349310031264&psc=1&mcid=c60252f1df633f8cafbeab5795ab3c5c&gad_source=1)
- OLED display (128x64 pixels) [BUY HERE](https://www.amazon.it/dp/B078J78R45/ref=sspa_dk_detail_2?pd_rd_i=B078J78R45&pd_rd_w=5wNUR&content-id=amzn1.sym.10c44ba0-99a9-46d2-be7c-ad851b65838d&pf_rd_p=10c44ba0-99a9-46d2-be7c-ad851b65838d&pf_rd_r=P9WM066EMKMMMB1BPJJ1&pd_rd_wg=HmjmV&pd_rd_r=1f808f12-ca05-4b66-a88b-d1aa4ce0c32e&s=electronics&sp_csd=d2lkZ2V0TmFtZT1zcF9kZXRhaWw&th=1)
- Joytisck [BUY HERE](https://www.amazon.it/dp/B06WRRKS9G?psc=1&ref=ppx_yo2ov_dt_b_product_details)
- Buzzer [BUY HERE](https://www.amazon.it/dp/B07ZTR24HX?ref=ppx_yo2ov_dt_b_product_details&th=1)
- Connecting wires [BUY HERE](https://www.amazon.it/dp/B0BRMKX5RT?ref=ppx_yo2ov_dt_b_product_details&th=1)

### Software
- [Arduino IDE](https://www.arduino.cc/en/software)

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

## Schema connections
buzzer 13
oled 12
vrx a0
vry a1
sda 20
scl 21
coming soon the schema

## Contributing
Contributions are welcome! If you have any improvements or bug fixes, feel free to open a pull request or submit an issue. Please ensure that your contributions align with the overall style and aim of the project.

## License
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
This project is licensed under the MIT License - see the [LICENSE](license.md) file for details.

## Acknowledgments
- Inspired by the classic Tetris game.

Thank you for checking out Tetris_Arduino! If you have any questions or need further assistance, feel free to open an issue or contact the project maintainer.
