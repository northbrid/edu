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

uint8_t fb[12] = {0}; 

void updateFB() {
  memset(fb, 0, 12);
  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 12; x++)
      if (frame[y][x])
        fb[(y * 12 + x) >> 3] |= (uint8_t)(1u << ((y * 12 + x) & 7));
}

void mpxStep() {
  static volatile int i_isr = 0;
  if ((fb[i_isr >> 3] & (1 << (i_isr % 8))) != 0) {
    matrix.on(i_isr);
  } else {
    matrix.off(i_isr);
  }
  i_isr = (i_isr + 1) % 96;
}

void slowMotion() {
  volatile int mpxCount = 0;
  volatile int tmpCount = 0;
  volatile int delayMul = 1;
  updateFB();
  noInterrupts();
  while(true) {
    mpxStep();
    mpxCount++;
    if (mpxCount >= 3000/delayMul) {
      delayMul+=1;
      mpxCount=0;
    }
    for(int i=0; i<3000*delayMul; i++) {
      tmpCount++;
    }
  }
}

