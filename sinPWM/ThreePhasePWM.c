#include <Arduino.h>
#include <avr/interrupt.h>
#include <math.h>

#define pi 3.1415926535897932384626433832795
#define TOP0 256
#define TOP1 256
#define TOP2 256
#define M 0.6
#define F 50.0

// PWM 1-2 = PIN 5-6
// PWM 3-4 = PIN 9-10
// PWM 5-6 = PIN 3-11

// PWM pin 10 = 5 = 3
// PWM pin 11 = 9 = 6

volatile double t = 0.0;
volatile float data1, data2, data3;
// volatile int TEST = 0;
// char buf[12];

ISR (TIMER2_OVF_vect){        //TIMER2_OVF_vect 
    TCNT0 = 0;
    TCNT1 = 0;
    TCNT2 = 0; 
    
    data1 = (M * TOP1 * sin(2.0*PI*F*t) + TOP1) / 2;
    data2 = (M * TOP1 * sin(2.0*PI*F*t + 2.0*pi/3.0) + TOP1) / 2;
    data3 = (M * TOP1 * sin(2.0*PI*F*t + 4.0*pi/3.0) + TOP1) / 2;
    OCR0A = data1;           // PWM Pin 6 
    OCR0B = data1;           // PWM Pin 5 inverted
    OCR1A = data2;           // PWM Pin 9
    OCR1B = data2;           // PWM Pin 10 inverted
    OCR2A = data3;           // PWM Pin 11
    OCR2B = data3;           // PWM Pin 3 inverted

        // Serial.println(data1);
        // Serial.println(data2);
        // Serial.println(data3);
    // f2str(data, buf);
    // Serial.println(buf);

    t += 0.000016;
    if (t >= 0.124980){
        t = 0.0;
    }
}

void setup() {
    // Serial.begin(115200);

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

    // PWM by timer 0 ----------------------------------------------------------------
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

    // PWM by timer 1 --------------------------------------------------------------------
    TCCR1A = 0; TCCR1B = 0; // Reset 2 registers
    DDRB |= (1 << PB2);     // PB2 is OUTPUT (pin 10)
    DDRB |= (1 << PB1);     // PB1 is OUTPUT (pin 9 )   
    
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12)|(1 << WGM13); // select Fast PWM mode select TOP_value freely ICR1                     
    // TCCR1A |= (1 << COM1B1); // None-inverted mode
    TCCR1A |= (1 << COM1A1);    // None-inverted mode Pin 9
    TCCR1A |= (1 << COM1B1) | (1 << COM1B0);  // inverted mode Pin 10

    ICR1 = TOP1 - 1;              // Frequency = 16M / TOP
    TCCR1B |= (1 << CS10);   // No Prescaling = F_Clock or F_clock/1=16mhz
}

void loop() {
}
