#include <Arduino.h>
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

char* VIDEORAM = (char*)0x20004000;

uint8_t frame[8][12] = {
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0},
};

char initialized = 0;

void updateMatrix() {
  if (initialized == 0) {
    matrix.begin();
    matrix.renderBitmap(frame, 8, 12);
    for(int i=0; i<12; i++) {
      *(volatile char*)(0x20004000+i) = (char)0;
    }
    initialized = 1;
  }
  for (int col = 0; col < 12; col++) {
    char columnData = VIDEORAM[col];
    
    for (int row = 0; row < 8; row++) {
      if (columnData & (1 << row)) {
        frame[row][col] = 1;
      } else {
        frame[row][col] = 0;
      }
    }
  }
  matrix.renderBitmap(frame, 8, 12);
};
