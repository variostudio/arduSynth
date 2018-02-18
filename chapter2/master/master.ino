#include <Wire.h>

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  randomSeed(analogRead(0));
}

void loop() {
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(random(17*4, 60*4));              // sends one byte
  Wire.endTransmission();    // stop transmitting

  delay(500);
}
