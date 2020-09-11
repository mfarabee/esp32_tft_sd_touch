#include "tftSD.h"
#include "tftDisplay.h"
extern TFT_CLASS *Tft;

// Sample routines to access SD card

void volumeSD(){
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluKB\n", SD.usedBytes() / 1024);
}

// List SD contents on TFT display (top level routine)
void listSDTft(String dir){
  File root;
  Tft->setTextSize(2);
  if(dir==""){
    dir="/";
  }
  root=SD.open(dir.c_str());
  listSDTft(root,0);
  root.close();
}


void listSDTft(File dir,int spaces){
  uint8_t i;
  File entry;
  
  if(dir!=0){
    dir.rewindDirectory(); // This is needed to reset the dir pointer
    for (i=0;i<spaces;++i) { Tft->print(' '); }
    Tft->print(dir.name());
    if(dir.isDirectory()){
     if(strcmp(dir.name(),"/") !=0){
        Tft->println("/");
     }else{
      Tft->println();
     }
    }else{
      Tft->print("        ");
      Tft->println(dir.size(),DEC);
    }
    entry=dir.openNextFile();
    spaces=spaces+4;
    while(entry != 0){
      if(entry.isDirectory()){
       listSDTft(entry,spaces);
      }else{
        for (i=0;i<spaces;++i) { Tft->print(' '); }
          Tft->print(entry.name());
          Tft->print("\t\t");
          Tft->println(entry.size(),DEC);
        }
        entry.rewindDirectory(); // This is needed to reset the dir pointer for future access
        entry.close();
        entry=dir.openNextFile();
    }
  }
}


// List SD contents to serial terminal (top level routine)
void listSD(String dir){
  File root;
  if(dir==""){
    dir="/";
  }
  root=SD.open(dir.c_str());
  listSD(root,0);
  root.close();
}

void listSD(File dir,int spaces){
  uint8_t i;
  File entry;
  
  if(dir!=0){
    dir.rewindDirectory(); // This is needed to reset the dir pointer
    for (i=0;i<spaces;++i) { Serial.print(' '); }
    Serial.print(dir.name());
    if(dir.isDirectory()){
     if(strcmp(dir.name(),"/") !=0){
        Serial.println("/");
     }else{
      Serial.println();
     }
    }else{
      Serial.print("        ");
      Serial.println(dir.size(),DEC);
    }
    entry=dir.openNextFile();
    spaces=spaces+4;
    while(entry != 0){
      if(entry.isDirectory()){
       listSD(entry,spaces);
      }else{
        for (i=0;i<spaces;++i) { Serial.print(' '); }
          Serial.print(entry.name());
          Serial.print("\t\t");
          Serial.println(entry.size(),DEC);
        }
        entry.rewindDirectory(); // This is needed to reset the dir pointer for future access
        entry.close();
        entry=dir.openNextFile();
    }
  }
}


int existsSDFile(String fName){
  int result=1;
    if(!SD.exists((char *)fName.c_str())){
      Serial.print(fName);
      Serial.println(" does not exist!");
      result=0;
    }
    return(result);
}
void readSDFile(String fName){
  File dataFile;
  if(existsSDFile(fName)){
    dataFile=SD.open(fName.c_str());
    while(dataFile.available()){
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }
}
