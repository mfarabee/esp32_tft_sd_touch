# esp32_tft_sd_touch: Default code for my board

The program sets up the esp32 nodeMCU board to communicate with the 2.8" SPI TFT display as well as

enabling the onboard touch and SD card. In addition to getting the hardware all working,

this code also includes a module to create a keyboard.

Parts required:
--------------
* 2.8" SPI TFT LCD Display Touch Panel 240X320
* ESP-WROOM-32 ESP32 ESP-32S Development Board (nodeMcu)
* My PCB  (optional)
* SD card (optional)

 Libraries required:
 --------------------
 * Adafruit_GFX.h        // https://github.com/adafruit/Adafruit-GFX-Library
 * Adafruit_ILI9341.h    // https://github.com/adafruit/Adafruit_ILI9341
 * XPT2046_Touchscreen.h // https://github.com/PaulStoffregen/XPT2046_Touchscreen
 * SPI.h
 * SD.h 

COMPILE:
-----------
 Arduino IDE: Use NodeMCU-32s board
 
 Edit: esp32_tft_sd_touch.h to enable/disable SD card or bitmap reader 
 
 Additioanl sample functions are available
 * calibrate touch  (in tftDisplay class)
 * Show SD card info (in tftSD file)
 
 Execution of Sample program:
 -----------
 Open Serial window at 115200 baud
 
 Program will do the following:
 * Display sample Alpha keyboard (result will display in Serial window)
 * Display sample Numeric keyboard (result will display in Serial window)
 * Enter a sample touch drawing loop. (every release will change color)

