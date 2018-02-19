#include <Wire.h>

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  randomSeed(analogRead(0));
}

void loop() {
  Wire.beginTransmission(7); 
  Wire.write(random(17*4, 20*4));  //Random freq      
  Wire.write(random(0, 5));        //Random wave form, 0..4
  Wire.endTransmission();    

  delay(random(100, 900));

  Wire.beginTransmission(8); 
  Wire.write(random(20*4, 60*4));         
  Wire.write(random(0, 5));        //Random wave form, 0..4     
  Wire.endTransmission();  

  delay(random(500, 900));
}
