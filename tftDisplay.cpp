#include "tftDisplay.h"
#include <Adafruit_GFX.h>        // https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_ILI9341.h>    // https://github.com/adafruit/Adafruit_ILI9341
#include <XPT2046_Touchscreen.h> // https://github.com/PaulStoffregen/XPT2046_Touchscreen


// base font = 6x8
// TFT display  320x240   Base font = 6x8
// scale 1 = 53 columns x 30 rows , 1 char =  6x8
// scale 2 = 26 columns x 15 rows,  1 char = 12x16
// scale 3 = 17 columns x 10 rows,  1 char = 18x24
// scale 4 = 13 columns x  7 rows , 1 char = 24x32
// scale 5 = 10 columns x  6 rows , 1 char = 30x40
// scale 6 =  8 columns x  5 rows , 1 char = 36x48


TFT_CLASS::TFT_CLASS(uint8_t useSD): Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST) {
  initDisplay(0);
  initTouch();
  if(useSD==1){
    initSD();
  }
}
TFT_CLASS::TFT_CLASS(uint8_t useSD, uint8_t rotation) : Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST) {
  initDisplay(rotation);
  initTouch();
  if(useSD==1){
     initSD();
  }
}

void TFT_CLASS::initDisplay(uint8_t rotation) {
  cp437(true); // Fixes bug with missing character at location 247. This should always be used
  begin();
  setRotation(rotation);
  fillScreen(ILI9341_BLACK);
  setTextColor(ILI9341_WHITE);
  setTextSize(2);
}
void TFT_CLASS::initTouch() {
  touchPtr = new XPT2046_Touchscreen(TOUCH_CS);
  touchPtr->setRotation(1);  // key touch rotation = 1,  Will do calculation in isTouched routine
}

void TFT_CLASS::initSD() {
  bool sdValid = false;
  
  // The following link shows where I got the information on how to configure a new SPI
  // https://github.com/espressif/arduino-esp32/blob/master/libraries/SPI/examples/SPI_Multiple_Buses/SPI_Multiple_Buses.ino  
  sdSpi = new SPIClass(HSPI); // setup for SD card SPI
  sdSpi->begin(SD_CLK,SD_MISO,SD_MOSI,SD_CS); // sck,miso,mosi,ss
  //sdSpi->begin(); // Using default HSPI

  // Initialize SD card and read contents
  sdValid = SD.begin(SD_CS, *sdSpi);
  fillScreen(ILI9341_BLACK);
  setCursor(0,0);

  if (sdValid) {
    Serial.println("SD succeeded!");
    println("SD succeeded!");
    //Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024)); // debug
    //Serial.printf("Used space: %lluKB\n", SD.usedBytes() / 1024);  // debug
  } else {
    Serial.println("SD failed!");
    println("SD failed!");
  }
  delay(5000);
}


// This routine converts raw touch data to pixel coordinates based on rotation
// TouchTop and TouchBottom X/Y coordinates are set in the tftDisplay.h. 
int TFT_CLASS::isTouched(int *x, int *y, int ignore) {
  int result = 0, tmpX, tmpY;
  static unsigned long touchDelay = millis();

  if ((millis() - touchDelay > TOUCH_DELAY) && touchPtr->touched()) {
    TS_Point p = touchPtr->getPoint();
    //Serial.print("Z:"); Serial.print(p.z); Serial.print(" X:"); Serial.print(p.x); Serial.print(" Y:"); Serial.println(p.y);
    if (ignore != 0 || (p.z > TOUCH_MIN_PRESSURE && p.z < TOUCH_MAX_PRESSURE) ) { // fix touchy display

      switch (getRotation()) {
        case 0:
          tmpY = map(p.y, TouchBottomY, TouchTopY, 0, width() - 1);
          tmpX = map(p.x, TouchLeftX, TouchRightX, 0, height() - 1);
          *x = constrain(tmpY, 0, width());
          *y = constrain(tmpX, 0, height());
          break;
        case 1:
          tmpX = map(p.x, TouchLeftX, TouchRightX, 0, width() - 1);
          tmpY = map(p.y, TouchTopY, TouchBottomY, 0, height() - 1);
          *x = constrain(tmpX, 0, width());
          *y = constrain(tmpY, 0, height());
          break;
        case 2:
          tmpY = map(p.y, TouchTopY, TouchBottomY, 0, width() - 1);
          tmpX = map(p.x, TouchRightX, TouchLeftX, 0, height() - 1);
          *x = constrain(tmpY, 0, width());
          *y = constrain(tmpX, 0, height());
          break;
        case 3:
          tmpX = map(p.x, TouchRightX, TouchLeftX, 0, width() - 1);
          tmpY = map(p.y, TouchBottomY, TouchTopY, 0, height() - 1);
          *x = constrain(tmpX, 0, width());
          *y = constrain(tmpY, 0, height());
          break;
      }

      result = p.z;
    }
    touchDelay = millis();
  }
  return (result);
}

XPT2046_Touchscreen *TFT_CLASS::getTouchPtr() {
  return (touchPtr);
}

// This function rotates the screen, but leaves the touch at rotation 1.
// The touch library could rotate the touch coordinates also, but hardcodes the
// touch values, causing inaccuracies. Because of this I do not use the touch library and
// do the calculations here based on screen rotation.
void TFT_CLASS::setRotation(uint8_t rotate) {
  rotation = constrain(rotate, 0, 3); // keep track of rotation
  Adafruit_ILI9341::setRotation(rotation);  // set screen rotation

  fillScreen(ILI9341_BLACK);
  setCursor(0, 0);
}

// this gets raw touch point locations (no additional conversion to pixels) used for calibration box.
int TFT_CLASS::getMyLocation(int *x, int *y) {
  int result = 0;
  if (touchPtr->touched()) {
    TS_Point p = touchPtr->getPoint();
    if (p.z > TOUCH_MIN_PRESSURE && p.z < TOUCH_MAX_PRESSURE ) { // fix touchy display
      *x = p.x;
      *y = p.y;
      result = 1;
    }
  }
  return (result);
}

void TFT_CLASS::calibrateBox(int x, int y, int *x1, int *y1) {
  drawFastHLine(x - 5, y, 10, ILI9341_WHITE);
  drawFastVLine(x, y - 5, 10, ILI9341_WHITE);
  while (getMyLocation(x1, y1) == 0) {
    delay(50);
  }
  drawFastHLine(x - 5, y, 10, ILI9341_BLACK);
  drawFastVLine(x, y - 5, 10, ILI9341_BLACK);
}

// The calibration may or may not work depending on how the touch is implemented (X-Y direction)
// The touch librray uses rotation 1 as default (notice coordinate system), so that is what I am using.
// For my TFT display the touch X direction is the long (320 pixel) direction. This calibrarion routine
// will get you close for the default values to put in the tftDisplay.h file, but you may want to tweak them
// to get the best accuracy across all rotations.
// See notes about rotation under the setRotation member function.
// This code could be modified to allow modification of Touch X/Y for realime recalibration
void TFT_CLASS::calibrate() {
  int x1, y1, x2, y2, x3, y3, x4, y4;
  float delta;
  int X_LEFT, X_RIGHT, Y_TOP, Y_BOT;

  fillScreen(ILI9341_BLACK);
  setRotation(1);
  calibrateBox(20, 20, &x1, &y1);
  delay(1000);
  calibrateBox(width() - 20, 20, &x2, &y2);
  delay(1000);
  calibrateBox(20, height() - 20, &x3, &y3);
  delay(1000);
  calibrateBox(width() - 20, height() - 20, &x4, &y4);

  delta = abs((abs(x4 - x1) + abs(x3 - x2)) / 2) / (width() - 40);
  if (x1 < x4) {
    X_LEFT = x1 - (delta * 20.0);
    X_RIGHT = x4 + (delta * 20.0);
  } else {
    X_LEFT = x1 + (delta * 20.0);
    X_RIGHT = x4 - (delta * 20.0);
  }
  delta = abs((abs(y4 - y1) + abs(y3 - y2)) / 2.0) / (height() - 40.0);
  if (y1 < y4) {
    Y_TOP = y1 - (delta * 20.0);
    Y_BOT = y4 + (delta * 20.0);
  } else {
    Y_TOP = y1 + (delta * 20.0);
    Y_BOT = y4 - (delta * 20.0);
  }
  setCursor(0, 0);
  print("CALIBRATION");
  setTextSize(2);
  setCursor(0, 40);
  print("TouchLeftX  = ");println(X_LEFT);
  print("TouchRightX = ");println(X_RIGHT);
  print("TouchTopY   = ");println(Y_TOP);
  print("TouchBottomY= ");println(Y_BOT);
  println();print("Touch anywhere to continue");
  Serial.print("X_LEFT="); Serial.println(X_LEFT);
  Serial.print("X_RIGHT="); Serial.println(X_RIGHT);
  Serial.print("X="); Serial.println(width());
  Serial.print("Y_TOP="); Serial.println(Y_TOP);
  Serial.print("Y_BOT="); Serial.println(Y_BOT);
  Serial.print("Y="); Serial.println(height());
  delay(200);
  while(getMyLocation(&x1,&y1) == 0){
        delay(50);
  }
}
