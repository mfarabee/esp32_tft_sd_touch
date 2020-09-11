#ifndef TFTDISPLAY_H
#define TFTDISPLAY_H

//https://forum.arduino.cc/index.php?topic=509750.0
#include <Adafruit_ILI9341.h>     // https://github.com/adafruit/Adafruit_ILI9341
#include <XPT2046_Touchscreen.h>  // https://github.com/PaulStoffregen/XPT2046_Touchscreen
#include <SPI.h>
#include <SD.h>

// There can be a problem connecting TFT,SD and touch using the same SPI bus. The 2.8 TFT 240x320 V1.2 displays
// directly connect the SD card to the SPI pins. This unfortunately does not tristate the MISO pin when the SD card is not in use.
// To solve this (and not adding a tristate driver chip, I put the SD card on a separate SPI bus (HSPI).

//https://github.com/espressif/esptool/wiki/ESP32-Boot-Mode-Selection
// ESP32 reset can be problematic if an invalid bootmode is set. This can happen if GPIO02 or GPIO12 is set incorrectly
// at time of reset. Originally I use default SD_MISO (GPIO12) and TFT_DC (GPIO02). This caused reset issues when an SD card was installed.
// I would get it to an invalid boot state (HSPI_FAST_BOOT or SPI_FAST_BOOT) and it would be an infinite loop of reboots.
// I fixed this by changing the SD_MISO pin from GPIO12 to GPIO26


//   HSPI standard pinouts
//  Default configuration SCLK = 14, MISO = 12, MOSI = 13, SS = 15
#define SD_CLK 14
#define SD_MISO 26  // Change from default pin to allow for reset without issues
#define SD_MOSI 13
#define SD_CS 15

// VSPI standard pinouts
// Default configuration SCLK = 18, MISO = 19, MOSI = 23, SS = 5
#define TFT_CS 5 
#define TFT_DC 33
#define TFT_RST 27
#define TFT_MOSI  23
#define TFT_SCLK 18
#define TFT_MISO 19
#define TOUCH_CS 4

#define TouchDefaultLeftX   3900
#define TouchDefaultRightX   320
#define TouchDefaultTopY 3915
#define TouchDefaultBottomY     260

// The following can be used to control the sensitivity of the touch screen
#define TOUCH_DELAY 10L
#define TOUCH_MIN_PRESSURE 500
#define TOUCH_MAX_PRESSURE 2500

/* Here are the colors defined in the Adafruit GFX library
#define ILI9341_BLACK       0x0000  ///<   0,   0,   0
#define ILI9341_WHITE       0xFFFF  ///< 255, 255, 255
#define ILI9341_NAVY        0x000F  ///<   0,   0, 123
#define ILI9341_DARKGREEN   0x03E0  ///<   0, 125,   0
#define ILI9341_DARKCYAN    0x03EF  ///<   0, 125, 123
#define ILI9341_MAROON      0x7800  ///< 123,   0,   0
#define ILI9341_PURPLE      0x780F  ///< 123,   0, 123
#define ILI9341_OLIVE       0x7BE0  ///< 123, 125,   0
#define ILI9341_LIGHTGREY   0xC618  ///< 198, 195, 198
#define ILI9341_DARKGREY    0x7BEF  ///< 123, 125, 123
#define ILI9341_BLUE        0x001F  ///<   0,   0, 255
#define ILI9341_GREEN       0x07E0  ///<   0, 255,   0
#define ILI9341_CYAN        0x07FF  ///<   0, 255, 255
#define ILI9341_RED         0xF800  ///< 255,   0,   0
#define ILI9341_MAGENTA     0xF81F  ///< 255,   0, 255
#define ILI9341_YELLOW      0xFFE0  ///< 255, 255,   0
#define ILI9341_ORANGE      0xFD20  ///< 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5  ///< 173, 255,  41
#define ILI9341_PINK        0xFC18  ///< 255, 130, 198
*/

class TFT_CLASS: public Adafruit_ILI9341{
  private:
    void initDisplay(uint8_t rotation);
    void initTouch();
    void initSD();
    void calibrateBox(int x,int y,int *x1, int *y1);
    SPIClass * sdSpi =NULL; // Used for SD card

    // The following are created as variables in case the values want to be modified on the fly (recalibrate)
    int TouchRightX=TouchDefaultRightX;
    int TouchLeftX=TouchDefaultLeftX;
    int TouchTopY=TouchDefaultTopY;
    int TouchBottomY=TouchDefaultBottomY;

    int getMyLocation(int *x, int *y);
        
  public:
    TFT_CLASS(uint8_t useSD);
    TFT_CLASS(uint8_t useSD, uint8_t rotation);
    XPT2046_Touchscreen *touchPtr;

    void setRotation(uint8_t rotation);
    int isTouched(int *x, int *y, int ignore=0);
    void calibrate();
    XPT2046_Touchscreen *getTouchPtr();
};


#endif
