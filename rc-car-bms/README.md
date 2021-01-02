# BMS for RC car battery

## Installation

Burn correct fuses using Arduino as ISP

    platformio run --target fuses -e attiny85bootloader

Install bootloader using Arduino as ISP

    platformio run --target bootloader -e attiny85bootloader

Upload main sketch using serial adapter

    platformio run --target upload -e attiny85