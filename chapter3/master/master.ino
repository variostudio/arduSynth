#include <Wire.h>

byte note = 6;

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  randomSeed(analogRead(0));
}

void loop() {
  Wire.beginTransmission(7);
  Wire.write(note * 4); //Random freq
  Wire.write(random(0, 5));        //Random wave form, 0..4
  Wire.endTransmission();

  delay(500);

  Wire.beginTransmission(7);
  Wire.write(0);
  Wire.write(1);        //Random wave form, 0..4
  Wire.endTransmission();

  delay(1000);


  note++;
  if (note > 40) {
    note = 6;
  }
}
