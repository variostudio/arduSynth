#include <Wire.h>
#include "notes.h"

#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) )

byte note = 6;
byte waveForm = 1;
int arrSize = 0;
int i = 0;

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
}

void loop() {
  arrSize = arr_len(HIGH_NOTES);
  for (i = 0; i < arrSize; i++) {
    bitSet(waveForm, 7);

    sendNote(HIGH_NOTES[i], waveForm);
  }

  arrSize = arr_len(LOW_NOTES);
  for (i = 0; i < arrSize; i++) {
    bitClear(waveForm, 7);

    sendNote(LOW_NOTES[i], waveForm);
  }  
}

void sendNote(int code, byte form) {
  Wire.beginTransmission(7);
  Wire.write(code);
  Wire.write(form);
  Wire.endTransmission();

  delay(500);

  Wire.beginTransmission(7);
  Wire.write(0);
  Wire.write(1);
  Wire.endTransmission();

  delay(1000);
}

