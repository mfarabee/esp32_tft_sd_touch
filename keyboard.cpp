#include "keyboard.h"

/*  Keyboard: These classes (KEY_CLASS ,KEYBOARD_CLASS) allow the programmer to create a keyboard for data entry.
 * A keyboard can consist of up to 3 screens containing 35 user defined keys per screen. Each screen will be populated
 * with five additional keys (Cancel,Enter,Backspace,Shift,Clear) as well as an entry box to display what is typed.
 * 
 * The programmer can control the key color, text color as will as the number of rows and columns. It is up to the user
 * To make sure that the row/column entries will suppoert the number of keys defined (row * col >= kumber of keys).
 * 
 * One the keyboard is defined, a call to activateKeyboard will display the keyboard and return a string containing the
 * resulting input. A keyboard only needs to be created once, but can be activated many times.
*/

#define EntrySize 40  // height of entry box in pixels
#define EntryScale 3  // text scale for entery box
#define KbdBackgroundColor  ILI9341_BLACK
#define EMPTY_CHAR 0x0
#define CANCEL_CHAR 0x01
#define BACKSPACE_CHAR 0x08
#define ENTER_CHAR 0X0D
#define SHIFT_CHAR 0x18
#define CLEAR_CHAR 0xAE

typedef struct {
  char value;
  char label[7];
} KbdStruct;
const KbdStruct LastRow[] ={{SHIFT_CHAR,{SHIFT_CHAR,'\0'}},{CANCEL_CHAR,"Cncl"},{BACKSPACE_CHAR,"Bksp"},{ENTER_CHAR,"Enter"}};

/****************************
          KEY CLASS
****************************/
// KEY_CLASS
//Constructor for Key Class
KEY_CLASS::KEY_CLASS(TFT_CLASS *ptr,int x,int y, int w, int h,char value,const char *kl){
  TftPtr=ptr;
  keyWidth=w-x; keyHeight=h-y; keyValue=value;
  strncpy(keyLabel,kl,6);
  keyLabel[6]= '\0'; // Put end of string char, since strncpy will not do this
  xl=x; xr=w; yt=y; yb=h;
  // Determine scale factor for text based on size of key
  scale=min(((keyWidth-2)/((int)strlen(keyLabel)*6)),(keyHeight-2)/8);
  // Calulate text location from edge of box (center text)
  textXoffset=(keyWidth-((strlen(keyLabel)*scale*6)-1))/2;
  textYoffset=(keyHeight-((scale*8)-1))/2;
}

// KEYDRAW
// Draw single key based on stored information
void KEY_CLASS::keyDraw(int textColor,int backColor){
  TftPtr->fillRoundRect(xl,yt, keyWidth-2, keyHeight-2,5,backColor);
  TftPtr->setTextColor(textColor);
  TftPtr->setTextSize(scale);
  TftPtr->setCursor(xl+textXoffset,yt+textYoffset);
  TftPtr->print(keyLabel);
}

// KEY TOUCHED
// Check if touch point is within the boundary of key location.
// Will "flash" key by reversing text and key colors when key is pressed
char  KEY_CLASS::keyTouched(int x,int y, int textColor, int keyColor){
  char result='\0';
  if(x>=xl && x<=xr && y>=yt && y<=yb){
    keyDraw(keyColor,textColor);
    delay(100);
    keyDraw(textColor,keyColor);
    result=keyValue;
    delay(200);  // delay to slow rapid entry if key is helpd down (repeat charater speed)
  }
  return(result);
}

/*****************************
         KEYBOARD CLASS
*****************************/

// KEYBOARD CLASS
// constructor for KEYBOARD Class
KEYBOARD_CLASS::KEYBOARD_CLASS(TFT_CLASS *ptr,int rows,int cols,int fg,int bg,  char *screenA,  char *screenB, char *screenC){
  int col,row,i,cnt,lastWidth,lastCnt,screen;
  char label[2];
  char *screens[3];
  
  TftPtr=ptr;
  TextColor=fg; KeyColor=bg;
  MaxEntryChars=(TftPtr->width()-EntrySize)/(EntryScale*6);

  // Determine key count for each of the three possible screens
  ScreenCount=0;
  if(strlen(screenA)>0){
  screens[ScreenCount]=screenA;
  KeyCount[ScreenCount]=strlen(screenA);
  ++ScreenCount;
  }
  if(strlen(screenB)>0){
  screens[ScreenCount]=screenB;
  KeyCount[ScreenCount]=strlen(screenB);
  ++ScreenCount;
  }
  if(strlen(screenC)>0){
  screens[ScreenCount]=screenC;
  KeyCount[ScreenCount]=strlen(screenC);
  ++ScreenCount;
  }

  // Calculate key height based on row and columns
  keyWidth=TftPtr->width()/cols;  
  keyHeight=(TftPtr->height()-EntrySize)/(rows+1);  

  // Loop through screen keys and create key instances at the proper location
  for(screen=0;screen<ScreenCount;++screen){  
    col=0;row=0;
    for(i=0;i<KeyCount[screen];++i){
      label[0]=screens[screen][i];label[1]='\0';
      KeyList[screen][i]=new KEY_CLASS(TftPtr,(col*keyWidth),(row*keyHeight)+EntrySize,(col*keyWidth)+keyWidth,(row*keyHeight)+EntrySize+keyHeight,screens[screen][i],label);
      ++col;
      if(col>=cols){
        col=0;
        ++row;
      }
    }

    // Add the CLEAR key on the entry line
    label[0]=CLEAR_CHAR;label[1]='\0';
    KeyList[screen][KeyCount[screen]]=new KEY_CLASS(TftPtr,TftPtr->width()-EntrySize,0,TftPtr->width(),EntrySize,CLEAR_CHAR,label);
    ++KeyCount[screen];
    
    cnt=0;
    i=KeyCount[screen];

    // Add the last row
    lastCnt=sizeof(LastRow)/sizeof(KbdStruct);
    KeyCount[screen] +=lastCnt;
    if(ScreenCount <=1){ // if there is only one screen, do not include shift key (first key in struct)
      --KeyCount[screen];
      ++cnt;
      lastWidth=TftPtr->width()/(lastCnt-1);
    }else{
       lastWidth=TftPtr->width()/lastCnt;
    }
    col=0;
    row=(rows*keyHeight)+EntrySize;
    while(cnt<lastCnt){
      KeyList[screen][i]=new KEY_CLASS(TftPtr,(col*lastWidth),row,(col*lastWidth)+lastWidth,row+keyHeight,LastRow[cnt].value,LastRow[cnt].label);
        ++col;
        ++i;
        ++cnt;
      }    
  }
}

// ACTIVATE KEYBOARD
// public function that draws keyboard and returns result
String KEYBOARD_CLASS::activateKeyboard(String label){
  drawKeyboard();
  populateEntry(label);
  return(getEntry());
}

// DRAW KEYBOARD
// Draws keys based on current screen
void KEYBOARD_CLASS::drawKeyboard(){
  int i;
  TftPtr->fillScreen(KbdBackgroundColor);
  for(i=0;i<KeyCount[CurrentScreen];++i){
    KeyList[CurrentScreen][i]->keyDraw(TextColor,KeyColor);
  }
}

// CHECK KEYS
// Checks if key is pressed by checking X/Y against all keys on current screen
char KEYBOARD_CLASS::checkKeys(int x, int y){
  int i;
  char result=0;
  for(i=0;i<KeyCount[CurrentScreen];++i){
    result=KeyList[CurrentScreen][i]->keyTouched(x,y,TextColor,KeyColor);
    if(result != 0){
      //Serial.print("  checkKeys: ");Serial.println(result);
      break;
    }
  }
  return(result);
}

// POPULATE ENTRY
//prints entry data in entry line. If the line is too short, it will only display that ending subset of charaters
void KEYBOARD_CLASS::populateEntry(String myData){
  int startChar;
  TftPtr->fillRect(0,0, TftPtr->width()-50, EntrySize,KbdBackgroundColor);
  TftPtr->setTextSize(EntryScale);   
  TftPtr->setCursor(0,0);
  if(myData.length() <= MaxEntryChars){
    startChar=0;
  }else{
    startChar=myData.length()- MaxEntryChars;
  }
  TftPtr->print(myData.substring(startChar));
}

// GET ENTRY
// If key is pressed, determine if it is data or a special charater that needs special handling (cancel, clear, enter, etc...)
String KEYBOARD_CLASS::getEntry(){
  int foundEnter=0,pos=0;
  int x,y;
  String myData="";
  char result;
  while(foundEnter==0){
    if(TftPtr->isTouched(&x, &y)) {
      result=checkKeys(x,y);
      if(result== ENTER_CHAR){             // Enter
        foundEnter=1;
      }else if(result == BACKSPACE_CHAR){  // Backspace
            if(pos>0){
              myData.remove(myData.length()-1,1);
              --pos;
              populateEntry(myData);
            }
      }else if(result == CANCEL_CHAR){     // Cancel
        myData="";
        foundEnter=1;
      }else if(result == SHIFT_CHAR){      // Shift (new screen)
        ++CurrentScreen;
        if(CurrentScreen >= ScreenCount){
          CurrentScreen=0;
        }
        drawKeyboard();
        populateEntry(myData);
      }else if(result == CLEAR_CHAR){      // Clear
        TftPtr->fillRect(0,0, TftPtr->width()-50, EntrySize,KbdBackgroundColor);
        pos=0;
        myData="";
      }else if(result != 0){              // Data charater
        myData.concat(result);
        populateEntry(myData);
        ++pos;
      }
   }
  }
   TftPtr->fillScreen(KbdBackgroundColor);
   //Serial.print("result= ");Serial.println(myData);
   return(myData);
}
