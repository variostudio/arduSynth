#include <Wire.h>
/******** Load AVR timer interrupt macros ********/
#include <avr/interrupt.h>

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
void initNote(int code) {
  noteFinished = false;
  cli();
  decayI = 0;
  decayJ = 0;
  index = 0;
  OCR2A = code;             // sets the frequency of the generated wave

  for (int i=0; i<LENGTH; i++) {
    float v = (AMP*sin((PI2/LENGTH)*i));  // Calculate current entry
    wave[i] = int(v+OFFSET);              // Store value as integer
  }  
  sei(); 
}

void setup() {  
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  initNote(32*4);
  
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
    //delay(0);
  } 
  if (decayJ == 127) {
    decayJ = 0;
    //cli();
    noteFinished = true;
  }
  
  decayI++;
}
  

void receiveEvent(int howMany) {
  int code = Wire.read();    // receive byte as an integer

  if (code > 0) {
    initNote(code);
  }
}


