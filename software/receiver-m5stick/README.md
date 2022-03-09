# Tally Receiver for the M5StickC
Supports both the original M5StickC and the new M5StickC Plus

## Building
Install PlattformIO https://platformio.org/platformio-ide

Open this project folder and change the upload_port in your [platformio.ini](platformio.ini) to match your port and press upload

If you want to build for the M5StickC Plus, uncomment the two last lines in [platformio.ini](platformio.ini)

## Features

  * Onboarding Using Captive Portal
  * DHCP & Static IP config
  * Factory Reset by turning the M5Stick on while holding BtnB (the small one on the side that is not the power button) for 5 seconds.
  * ID can be set using the BtnA (Big one in front). When the ID stops blinking it will be persisted.