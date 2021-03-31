#include "LedControl.h"

// DIN auf pin7
// clk auch 6
// CS auf 5

LedControl lc=LedControl(7,6,5,1);
byte figure[8]={
  B00111100,
  B01000010,
  B10011001,
  B10111101,
  B10111101,
  B10011001,
  B01000010,
  B00111100};

void setup() {
  lc.shutdown(0,false);
  lc.setIntensity(0,2);
  lc.clearDisplay(0);
}

void loop() { 
  for (int i=0; i<8; i++){
    lc.setRow(0,i,figure[i]);
  }
  delay(20);
}  


/*
Weitere interessante Befehle:

lc.setColumn(0,col,B10100000);
lc.setLed(0,row,col,true);

*/
