/******** Load AVR timer interrupt macros ********/
#include <avr/interrupt.h>

/******** Sine wave parameters ********/
#define PI2     6.283185 // 2 * PI - saves calculating it later
#define AMP     127      // Multiplication factor for the sine wave
#define OFFSET  128      // Offset shifts wave to just positive values

/******** Lookup table ********/
#define LENGTH  256  // The length of the waveform lookup table
byte wave[LENGTH];   // Storage for the waveform
byte wave2[LENGTH];
byte index=0;    // Points to successive entries in the wavetable

void setup() {

 /******** Populate the waveform lookup table with a sine wave ********/
 for (int i=0; i<LENGTH; i++) {
   float v = (AMP*sin((PI2/LENGTH)*i));  // Calculate current entry
   wave2[i] = int(v+OFFSET);              // Store value as integer
 }
 /******** Set timer1 for 8-bit fast PWM output ********/
 pinMode(9, OUTPUT);       // Make timer's PWM pin an output
 TCCR1B  = (1 << CS10);    // Set prescaler to full 16MHz
 TCCR1A |= (1 << COM1A1);  // PWM pin to go low when TCNT1=OCR1A
 TCCR1A |= (1 << WGM10);   // Put timer into 8-bit fast PWM mode
 TCCR1B |= (1 << WGM12);

 /******** Set up timer 2 to call ISR ********/
 TCCR2A = 0;               // We need no options in control register A
 TCCR2B = (1 << CS20);     // Set prescaller to divide by 8
 TIMSK2 = (1 << OCIE2A);   // Set timer to call ISR when TCNT2 = OCRA2
 OCR2A = 31*4;             // sets the frequency of the generated wave
 sei();                    // Enable interrupts to generate waveform!
}

void loop() {  // Nothing to do!
  morph();
  decay();
  reset_note();
  //delay(200);
}

/******** Called every time TCNT2 = OCR2A ********/
ISR(TIMER2_COMPA_vect) {  // Called each time TCNT2 == OCR2A
 OCR1AL = wave[index++]; // Update the PWM output
 asm("NOP;NOP");         // Fine tuning
 TCNT2 = 6;              // Timing to compensate for time spent in ISR
}

void morph(void) {
 int i,j;
 for (j=0; j<128; j++) {
   for (i=0; i<LENGTH; i++) {
     if (wave[i] < wave2[i]) {
       wave[i]++;
     }
     if (wave[i] > wave2[i]) {
       wave[i]--;
     }
   }
   delay(10);
 }
}

void decay(void) {
  int i, j;
  for (j=0; j<128; j++) {
    for (i=0; i<LENGTH; i++) {
      if (wave[i]<127) {
        wave[i]++;
      }
      if (wave[i]>127) {
        wave[i]--;
      }
    }
    delay(10);
  }
}

void reset_note(void) {
  int i, j;
  for (j=0; j<128; j++) {
    for (i=0; i<LENGTH; i++) {
      if (wave[i]>0) {
        wave[i]--;
      }
    }
    delay(3);
  }
}


