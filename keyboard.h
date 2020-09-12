#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "tftDisplay.h"

class KEY_CLASS{
  private:
        TFT_CLASS *TftPtr;
        int xl,xr,yt,yb;
        int keyWidth,keyHeight;
        char keyValue,keyLabel[7];
        int scale,textXoffset,textYoffset;
  public:
    KEY_CLASS(TFT_CLASS *,int x,int y,int w,int h,  char value,const char *kl);
    char keyTouched(int x,int y,int textColor,int keyColor);
    void keyDraw(int textColor,int backColor);
};

class KEYBOARD_CLASS{
  private:
        TFT_CLASS *TftPtr;
        int keyWidth;
        int keyHeight;
        KEY_CLASS *KeyList[3][40];
        int CurrentScreen=0;
        int ScreenCount;
        int KeyCount[3]={0,0,0};
        int TextColor,KeyColor;
        int MaxEntryChars;
        
        char checkKeys(int x,int y);
        String getEntry();
        void populateEntry(String myData);
        void drawKeyboard();
  public:
    KEYBOARD_CLASS(TFT_CLASS *ptr,int rows,int cols, int fg, int bg, char *screenA,  char *screenB, char *screenC);
    String activateKeyboard(String label);
};

#endif
