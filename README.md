# Arduino Synthesizer project

This project is about generating good quality music with Arduino platform.
It consist of small chapters describing project iteration step by step

## Getting Started
The project is based on Arduino platform. Details could be found here [Arduino](https://www.arduino.cc/)

My special thanks to
* Jon Thompson and [Advanced Arduino Sound Synthesis](https://makezine.com/projects/make-35/advanced-arduino-sound-synthesis/) 
* [Arduino Penny Organ](https://www.youtube.com/watch?v=Ehlp5DnNiag)

## Chapter 1. Generating sounds with Arduino
For the beginning I strongly recommend take a look onto great article [Advanced Arduino Sound Synthesis](https://makezine.com/projects/make-35/advanced-arduino-sound-synthesis/) 
I would put the original code here with some adoptions.
So, it the **chapter1** folder two samples could be found - the original **listning2** from Jon Thompson article and my adoption for Arduino Pro Mini 3v 8MHz.  

Why Pro Mini 3v 8MHz?

Because it is:
* cheap
* power saving.
The Pro Mini board 3.3v version takes about 5 mA instead of 5V version (about 20mA)
[Arduino Low Power](http://www.home-automation-community.com/arduino-low-power-how-to-run-atmega328p-for-a-year-on-coin-cell-battery/)

But small code correction should be made because it runs at 8Mhz instead of 16 Mhz
So, we need to tweak timer a bit. Original code working at 16Mhz was:

```C
void setup() {
	....
 
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
```
This is prescaler setting
`TCCR2B = (1 << CS21);     // Set prescaller to divide by 8` 
makes timer works at 1/8 of original clock speed (Actually 16/8 = 2Mhz)
In the case of 8Mhz Pro Mini it would be 1Mhz... Not so fast. There is required to be a bit faster.
In the spec of Atmega 328P all prescaler values are listed:

| CA22 | CA21 | CS20 | Description |
| ---- | ---- | ---- | ----------- |
|0     |0     |0     |No clock source (Timer/Counter stopped) |
|0     |0     |1     |clk I/O /1 (No prescaling) |
|0     |1     |0     |clk I/O /8 (From prescaler) |
|0     |1     |1     |clk I/O /32 (From prescaler) |

So, there is no 1/4 clock speed value. This case I desided to not use prescaler (use clk/1) but use `OCR2A` value nultiplied by 4
The 3.3v version is:

```C
void setup() {

  /******** Populate the waveform lookup table with a sine wave ********/
  
  
  waveform(SINE);
 
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
  OCR2A = 32*4;               // sets the frequency of the generated wave
  sei();                    // Enable interrupts to generate waveform!
}
```

There is all differences with 5v and 3.3v versions.
