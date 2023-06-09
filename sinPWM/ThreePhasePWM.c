// #include <Arduino.h>
// #include <avr/interrupt.h>

// const int pwmPin1 = 3;   // PWM pin 1
// const int pwmPin2 = 5;   // PWM pin 2
// const int pwmPin3 = 6;   // PWM pin 3
// const int pwmPin4 = 9;   // PWM pin 4
// const int pwmPin5 = 10;  // PWM pin 5
// const int pwmPin6 = 11;  // PWM pin 6

// void setup() {
//   // Set the PWM pins as outputs
//   pinMode(pwmPin1, OUTPUT);
//   pinMode(pwmPin2, OUTPUT);
//   pinMode(pwmPin3, OUTPUT);
//   pinMode(pwmPin4, OUTPUT);
//   pinMode(pwmPin5, OUTPUT);
//   pinMode(pwmPin6, OUTPUT);

//   // Configure Timer1 for PWM generation
//   // TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1C1) | _BV(WGM10);
//   TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM10);
//   TCCR1B = _BV(WGM12) | _BV(CS10);

//   // Set the PWM frequency to 10 kHz
//   OCR1A = 79;   // Adjust this value for desired duty cycle (0-255)
//   OCR1B = 79;
//   // OCR1C = 79;
// }

// void loop() {
//   // No need for further code in the loop since we're using Timer1 for PWM generation
// }

#include <Arduino.h>
#include <avr/interrupt.h>
#include <math.h>

#define pi 3.1415926535897932384626433832795
#define TOP0 256
#define TOP1 256
#define TOP2 256
#define M 0.6

// PWM 1-2 = PIN 5-6
// PWM 3-4 = PIN 9-10
// PWM 5-6 = PIN 3-11

// PWM pin 10 = 5 = 3
// PWM pin 11 = 9 = 6


int f = 50;
volatile double t = 0.0;
volatile float data1, data2, data3;
volatile int TEST = 0;
// char buf[12];

ISR (TIMER2_OVF_vect){        //TIMER2_OVF_vect 
    TCNT0 = 0;
    TCNT1 = 0;
    TCNT2 = 0;
    
    data1 = (M * TOP1 * sin(2.0*pi*f*t) + TOP1) / 2;
    data2 = (M * TOP1 * sin(2.0*pi*f*t + 2.0*pi/3.0) + TOP1) / 2;
    data3 = (M * TOP1 * sin(2.0*pi*f*t + 4.0*pi/3.0) + TOP1) / 2;
    OCR0A = data1;           // PWM Pin 6 
    OCR0B = data1;           // PWM Pin 5 inverted
    OCR1A = data2;           // PWM Pin 9
    OCR1B = data2;           // PWM Pin 10 inverted
    OCR2A = data3;           // PWM Pin 11
    OCR2B = data3;           // PWM Pin 3 inverted

    // Serial.println(data1);
    // f2str(data, buf);
    // Serial.println(buf);
 
    t += 0.000016;
    if (t >= 0.124980){
        t = 0.0;
        // digitalWrite(2, TEST);
        // TEST = !TEST;
    }
}

void setup() {
    // Serial.begin(115200);
    // pinMode(2, OUTPUT);

    // PWM by timer 2 and Interrupt ---------------------------------------------------
    TCNT2 = 0;
    TCCR2A = 0; TCCR2B = 0;
    DDRD |= (1 << PD3);   // output Pin 3
    DDRB |= (1 << PB3);   // output Pin 11
    TCCR2A |= (1 << COM2A1);    // None-inverted Pin 11
    TCCR2A |= (1 << COM2B1) | (1 << COM2B0);    // inverted Pin 3
    TCCR2A |= (1 << WGM21) | (1 << WGM20);      // Xung rang cua
    TCCR2B |= (0 << CS22) | (0 << CS21) | (1 << CS20);  // Prescaler
    // OCR2A = TOP2 - 1;              // Frequence = (16000000/((TOP2 + 1) * Prescaler))
    
    //TIMSK2 |= (1 << OCIE2A);  // Output Compare Match A Interrupt Enable
                                // TIMER2_COMPA_vect
            
    TIMSK2 |= (1 << TOIE2); // Timer2 Overflow Interrupt Enable
    sei();                  // Enable global interrupts   

    // PWM by timer 0 ---------------------------------------------------
    TCCR0A = 0; TCCR0B = 0; // Reset 2 registers
    DDRD |= (1 << PD5);     // PD5 is OUTPUT (pin 5)
    DDRD |= (1 << PD6);     // PB1 is OUTPUT (pin 6 )   
    
    TCCR0A |= (1 << WGM01) | (1 << WGM00);
    TCCR0A |= (1 << COM0B1);  // Fast mode Pin 5
    TCCR0A |= (1 << COM0A1);  // Fast mode Pin 6
    TCCR0A |= (1 << COM0A1);    // None-inverted mode Pin 6
    TCCR0A |= (1 << COM0B1) | (1 << COM0B0);  // inverted mode Pin 5

    TCCR0B |= (1 << CS10);   // No Prescaling = F_Clock or F_clock/1=16mhz
    // Frequence = Fpwm / 256 = 62500 Hz
    // Top value = 256


    // PWM by timer 1 -------------------------------------------------
    TCCR1A = 0; TCCR1B = 0; // Reset 2 registers
    DDRB |= (1 << PB2);     // PB2 is OUTPUT (pin 10)
    DDRB |= (1 << PB1);     // PB1 is OUTPUT (pin 9 )   
    
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12)|(1 << WGM13); // select Fast PWM mode select TOP_value freely ICR1                     
    // TCCR1A |= (1 << COM1B1); // None-inverted mode
    TCCR1A |= (1 << COM1A1);    // None-inverted mode Pin 9
    TCCR1A |= (1 << COM1B1) | (1 << COM1B0);  // inverted mode Pin 10

    ICR1 = TOP1;              // Frequency 20kHz = 16M / TOP
    TCCR1B |= (1 << CS10);   // No Prescaling = F_Clock or F_clock/1=16mhz
}

void loop() {
}
