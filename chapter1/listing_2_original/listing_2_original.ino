/******** Load AVR timer interrupt macros ********/
#include <avr/interrupt.h>

/******** Sine wave parameters ********/
#define PI2     6.283185 // 2 * PI - saves calculating it later
#define AMP     127      // Multiplication factor for the sine wave
#define OFFSET  128      // Offset shifts wave to just positive values

/******** Lookup table ********/
#define LENGTH  256  // The length of the waveform lookup table
byte wave[LENGTH];   // Storage for the waveform

/******** Waveform parameters ********/
#define SINE     0
#define RAMP     1
#define TRIANGLE 2
#define SQUARE   3
#define RANDOM   4

void setup() {

  /******** Populate the waveform lookup table with a sine wave ********/
  for (int i=0; i<LENGTH; i++) {
    float v = (AMP*sin((PI2/LENGTH)*i));  // Calculate current entry
    wave[i] = int(v+OFFSET);              // Store value as integer
    waveform(RAMP);                      // Replace sine with the different cases to
                                        // Produce the different waves
  }
 
  /******** Set timer1 for 8-bit fast PWM output ********/
  pinMode(9, OUTPUT);       // Make timer's PWM pin an output
  TCCR1B  = (1 << CS10);    // Set prescaler to full 16MHz
  TCCR1A |= (1 << COM1A1);  // PWM pin to go low when TCNT1=OCR1A
  TCCR1A |= (1 << WGM10);   // Put timer into 8-bit fast PWM mode
  TCCR1B |= (1 << WGM12); 

  /******** Set up timer 2 to call ISR ********/
  TCCR2A = 0;               // We need no options in control register A
  TCCR2B = (1 << CS21);     // Set prescaller to divide by 8
  TIMSK2 = (1 << OCIE2A);   // Set timer to call ISR when TCNT2 = OCRA2
  OCR2A = 32;               // sets the frequency of the generated wave
  sei();                    // Enable interrupts to generate waveform!
}

void loop() {  // Nothing to do!
}

/******** Called every time TCNT2 = OCR2A ********/
ISR(TIMER2_COMPA_vect) {  // Called each time TCNT2 == OCR2A
  static byte index=0;    // Points to successive entries in the wavetable
  OCR1AL = wave[index++]; // Update the PWM output
  asm("NOP;NOP");         // Fine tuning
  TCNT2 = 6;              // Timing to compensate for time spent in ISR
}


void waveform(byte w) {
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
    break;

  case RANDOM:
    randomSeed(2);
    for (int i=0; i<LENGTH; i++) {
      wave[i]=random(255);
    }
      break;
  }
}

