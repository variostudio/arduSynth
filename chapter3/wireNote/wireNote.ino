#include <Wire.h>
/******** Load AVR timer interrupt macros ********/
#include <avr/interrupt.h>
#include "waveform.h"

/******** Sine wave parameters ********/
#define PI2     6.283185 // 2 * PI - saves calculating it later
#define AMP     127      // Multiplication factor for the sine wave
#define OFFSET  128      // Offset shifts wave to just positive values

/******** Lookup table ********/
#define LENGTH  256  // The length of the waveform lookup table
byte wave[LENGTH];   // Storage for the waveform
byte index = 0;
int decayI;
int decayJ;

boolean noteFinished = true;

/******** Populate the waveform lookup table with a sine wave ********/
void initNote(int code, int form) {
  noteFinished = false;
  cli();
  decayI = 0;
  decayJ = 0;
  index = 0;
  OCR2A = code;             // sets the frequency of the generated wave

  waveform(form);

  sei(); 
}

void setup() {  
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  initNote(32*4, SINE);
  
  /******** Set timer1 for 8-bit fast PWM output ********/
  pinMode(9, OUTPUT);       // Make timer's PWM pin an output
  
  TCCR1B  = (1 << CS10);    // Set prescaler to full 16MHz
  TCCR1A |= (1 << COM1A1);  // PWM pin to go low when TCNT1=OCR1A
  TCCR1A |= (1 << WGM10);   // Put timer into 8-bit fast PWM mode
  TCCR1B |= (1 << WGM12); 

  /******** Set up timer 2 to call ISR ********/
  TCCR2A = 0;               // We need no options in control register A
  TCCR2B = (1 << CS20);     // Set prescaller to divide by 1
  TIMSK2 = (1 << OCIE2A);   // Set timer to call ISR when TCNT2 = OCRA2
  sei();                    // Enable interrupts to generate waveform!
}

void loop()
{  
  decay();
}

/******** Called every time TCNT2 = OCR2A ********/
ISR(TIMER2_COMPA_vect) {  // Called each time TCNT2 == OCR2A
  OCR1AL = wave[index++]; // Update the PWM output
  asm("NOP;NOP");         // Fine tuning
  TCNT2 = 6;              // Timing to compensate for time spent in ISR
}

  
void decay() {
  if (wave[decayI]<127) {
    wave[decayI]++;
  }
  if (wave[decayI]>127) {
    wave[decayI]--;
  }
    
  if (decayI == LENGTH-1) {
    decayI = 0;
    decayJ++;
    delay(2);
  } else {
    decayI++;  
  }
  if (decayJ == 127) {
    decayJ = 0;
    //cli();
    noteFinished = true;
  }
}
  

void receiveEvent(int howMany) {
  int code = Wire.read();    // receive byte as an integer
  int form = Wire.read();

  if (code > 0) {
    initNote(code, form);
  }
}

void waveform(int w) {
 switch(w) {

   case SINE: 
    for (int i=0; i<LENGTH; i++) 
      {float v = OFFSET+(AMP*sin((PI2/LENGTH)*i));
      wave[i]=int(v);
    }
    break;

  case RAMP:
    for (int i=0; i<LENGTH; i++) {
      wave[i]=i;
    }
    break;

  case TRIANGLE:
    for (int i=0; i<LENGTH; i++) {
      if (i<(LENGTH/2)) {
        wave[i]=i*2;
      } else {
        wave[i]=(LENGTH-1)-(i*2);
      }
    }
    break;
    
  case SQUARE:
    for (int i=0; i<(LENGTH/2); i++) {
      wave[i]=255;
    }
    for (int i=(LENGTH/2)+1; i<LENGTH; i++) {
      wave[i]=0;
    }    
    break;

  case RANDOM:
    randomSeed(2);
    for (int i=0; i<LENGTH; i++) {
      wave[i]=random(255);
    }
    break;
  }
}
