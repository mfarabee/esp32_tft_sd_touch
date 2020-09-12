// For NodeMCU-32s and 240x320 TFT touch display with SD card
/*
Connections:
Display and Touch are using VSPI bus
DISPLAY    ESP32        NOTES
T_IRQ       NC          Not using IRQ
T_DO        NC          Touch Data Out
T_DIN       GPIO23      Touch Data In
T_CS        GPIO4       Touch Chip Select
T_CLK       GPIO18      Touch Clock
SDO(MISO)   GPIO19      TFT_MISO
LED         3.3V
SCK         GPIO18      TFT_SCLK
SDI(MOSI)   GPIO23      TFT_MOSI
DC          GPIO33      TFT_DC
RESET       GPIO27      TFT_RST
CS          GPIO05      TFT_CS
GND         GND
VCC         +5V (VIN)   The display has a voltage regulator requiring +5V input

SD card is using HSPI buss
SD_CS       GPIO15
SD_MOSI     GPIO13
SD_MISO     GPIO26      No Tristate driver on Display card for SPI interface
SD_SCK      GPIO14
*/

#include "esp32_tft_sd_touch.h"
#include "tftDisplay.h"
#include "tftSD.h"
#include "keyboard.h"

// Alphanumeric Keyboard (27 keys) recommendations for rotation=[1,2]=row=3 col=9 | for rotation=[0,3]=row=7 col=4
// Numeric Keyboard (12 keys) recommendations for rotation=[1,2] =row=3 col=4 | for rotation=[0,3]=row=4 col=3
char KbdLower[]  = "abcdefghijklmnopqrstuvwxyz ";
char KbdUpper[]  = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
char KbdSymbol[]= "1234567890!@#$%^&*-_:;,./+?";
char KbdNum[]    = "1234567890.-";
char KbdEmpty[]  = "";



TFT_CLASS *Tft;
XPT2046_Touchscreen *touch;

KEYBOARD_CLASS *kbd, *kbd2;

// Sample drawing routine
void drawIt() {
  int x, y,rotation;
  char inData;
  if (Serial.available() > 0) {
    inData = Serial.read();
    switch (inData) {
      case 'c':
        Tft->fillScreen(ILI9341_BLACK);
        break;
      case 'r':
        rotation = (Tft->getRotation()+1) % 4;
        Serial.print("ROT:"); Serial.println(rotation);
        Tft->setRotation(rotation);
        break;
    }
  }
  if (Tft->isTouched(&x, &y)) {
    Tft->fillCircle(x, y, 2, ILI9341_RED);
  }
}


void setup() {
  File root; // Needed for SD card

  String result="";
  Serial.begin(115200);
  Tft= new TFT_CLASS(SD_ENABLE,SCREEN_ROTATION); // Set  SD card=off/on  and Initial Rotation
  touch=Tft->getTouchPtr();  // touch pointer may not be needed at this level, but is accesible.
  
  // Tft->calibrate(); // un-comment to calibrate touch
  // listSDTft("");  // example of SD usage, un-comment to list SD card files to Tft (Set SD_USAGE = 1)
  // listSD("");  // example of SD usage, un-comment to list SD card files to terminal (Set SD_USAGE = 1)
  delay(1000);


  // Example of how to create keyboards
  Tft->fillScreen(ILI9341_BLACK);
  // Create 2 keyboards. This should only happen once
  kbd = new KEYBOARD_CLASS(Tft,3,9,ILI9341_WHITE,ILI9341_BLUE,KbdLower,KbdUpper,KbdSymbol);
  kbd2 = new KEYBOARD_CLASS(Tft,3,4,ILI9341_WHITE,ILI9341_BLUE,KbdNum,KbdEmpty,KbdEmpty);

  // Once a keyboard is created, it can be activated at any time
  result = kbd->activateKeyboard("Name?");
  Serial.print("result= ");Serial.println(result);
  result=kbd2->activateKeyboard("Zip Code?");
  Serial.print("result= ");Serial.println(result);
  
}


void loop() {
  drawIt();  // Sample drawing program
}
