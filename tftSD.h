#ifndef TFTSD_H
#define TFTSD_H
#include <SD.h>

void volumeSD();
void listSDTft(String dir);
void listSDTft(File dir,int spaces);
void listSD(String dir);
void listSD(File dir,int spaces);
int  existsSDFile(String fName);
void readSDFile(String fName);

#endif
