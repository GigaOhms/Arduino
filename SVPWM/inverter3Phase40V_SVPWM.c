#include <Arduino.h>
#include <avr/interrupt.h>
#include <math.h>

#define TOP0 256.0
#define F 50.0
#define SQRT3	1.7320508075688772935274463415059

// PWM 1-2 = PIN 5-6
// PWM 3-4 = PIN 9-10
// PWM 5-6 = PIN 3-11

// PWM pin 10 = 5 = 3
// PWM pin 11 = 9 = 6

volatile double t = 0.0, m = 0.7;
volatile int	phi, sect;
volatile double	T1, T2, T0;
volatile double	S1, S2, S3;
volatile int i = 0;
volatile byte TEST = 0;
// volatile int TEST = 0;
// char buf[12];

void SVPWM(void);

float va[101] = {0.5000, 0.5604, 0.6205, 0.6652, 0.7241, 0.7676, 0.8244, 0.8796, 0.9199, 0.9455, 0.9568, 0.9698, 0.9806, 0.9872, 0.9938, 0.9973, 0.9997, 0.9997, 0.9981, 0.9938, 0.9891, 0.9806, 0.9698, 0.9603, 0.9455, 0.9330, 0.9494, 0.9636, 0.9728, 0.9830, 0.9908, 0.9951, 0.9988, 0.9999, 0.9993, 0.9963, 0.9924, 0.9851, 0.9782, 0.9668, 0.9532, 0.9415, 0.9066, 0.8660, 0.8104, 0.7676, 0.7095, 0.6504, 0.6055, 0.5453, 0.5000, 0.4396, 0.3795, 0.3348, 0.2759, 0.2324, 0.1756, 0.1204, 0.0801, 0.0545, 0.0432, 0.0302, 0.0194, 0.0128, 0.0062, 0.0027, 0.0003, 0.0003, 0.0019, 0.0062, 0.0109, 0.0194, 0.0302, 0.0397, 0.0545, 0.0670, 0.0506, 0.0364, 0.0272, 0.0170, 0.0109, 0.0049, 0.0012, 0.0001, 0.0007, 0.0027, 0.0076, 0.0149, 0.0218, 0.0332, 0.0432, 0.0585, 0.0934, 0.1340, 0.1896, 0.2324, 0.2905, 0.3496, 0.3945, 0.4547};
float vb[101] = {1.0000, 0.9988, 0.9951, 0.9908, 0.9830, 0.9755, 0.9636, 0.9494, 0.9373, 0.8932, 0.8522, 0.7962, 0.7387, 0.6948, 0.6355, 0.5905, 0.5302, 0.4698, 0.4245, 0.3645, 0.3199, 0.2613, 0.2038, 0.1616, 0.1068, 0.0670, 0.0506, 0.0364, 0.0272, 0.0170, 0.0092, 0.0049, 0.0012, 0.0001, 0.0007, 0.0037, 0.0076, 0.0149, 0.0218, 0.0332, 0.0468, 0.0585, 0.0585, 0.0468, 0.0332, 0.0245, 0.0149, 0.0076, 0.0037, 0.0007, 0.0000, 0.0012, 0.0049, 0.0092, 0.0170, 0.0245, 0.0364, 0.0506, 0.0627, 0.1068, 0.1478, 0.2038, 0.2613, 0.3052, 0.3645, 0.4095, 0.4698, 0.5302, 0.5755, 0.6355, 0.6801, 0.7387, 0.7962, 0.8384, 0.8932, 0.9330, 0.9494, 0.9636, 0.9728, 0.9830, 0.9891, 0.9951, 0.9988, 0.9999, 0.9993, 0.9973, 0.9924, 0.9851, 0.9782, 0.9668, 0.9568, 0.9415, 0.9415, 0.9532, 0.9668, 0.9755, 0.9851, 0.9924, 0.9963, 0.9993};
float vc[101] = {0.0000, 0.0012, 0.0049, 0.0092, 0.0170, 0.0245, 0.0364, 0.0506, 0.0627, 0.0545, 0.0432, 0.0302, 0.0194, 0.0128, 0.0062, 0.0027, 0.0003, 0.0003, 0.0019, 0.0062, 0.0109, 0.0194, 0.0302, 0.0397, 0.0545, 0.0670, 0.1204, 0.1756, 0.2180, 0.2759, 0.3348, 0.3795, 0.4396, 0.4849, 0.5453, 0.6055, 0.6504, 0.7095, 0.7532, 0.8104, 0.8660, 0.9066, 0.9415, 0.9532, 0.9668, 0.9755, 0.9851, 0.9924, 0.9963, 0.9993, 1.0000, 0.9988, 0.9951, 0.9908, 0.9830, 0.9755, 0.9636, 0.9494, 0.9373, 0.9455, 0.9568, 0.9698, 0.9806, 0.9872, 0.9938, 0.9973, 0.9997, 0.9997, 0.9981, 0.9938, 0.9891, 0.9806, 0.9698, 0.9603, 0.9455, 0.9330, 0.8796, 0.8244, 0.7820, 0.7241, 0.6801, 0.6205, 0.5604, 0.5151, 0.4547, 0.4095, 0.3496, 0.2905, 0.2468, 0.1896, 0.1478, 0.0934, 0.0585, 0.0468, 0.0332, 0.0245, 0.0149, 0.0076, 0.0037, 0.0007};

ISR (TIMER1_OVF_vect){        //TIMER1_OVF_vect 
    TCNT0 = 0;
    TCNT2 = 0;

    OCR0A = ((va[i] - 0.5) * m + 0.5) * TOP0 + 0.5;           // PWM Pin 6
    OCR0B = ((vb[i] - 0.5) * m + 0.5) * TOP0 + 0.5;           // PWM Pin 5
    OCR2A = ((vc[i] - 0.5) * m + 0.5) * TOP0 + 0.5;           // PWM Pin 11
    
    i++;
    if (i >= 100){
        i = 0;
        digitalWrite(2, TEST);
        TEST = ~TEST;
    } 
}

void setup() {
    // Serial.begin(115200);
    pinMode(2, OUTPUT);

    cli();//stop interrupts

    // PWM by timer 0 ---------------------------------------------------
    TCNT0 = 0;
    TCCR0A = 0; TCCR0B = 0; // Reset 2 registers
    DDRD |= (1 << PD5);     // PD5 is OUTPUT (pin 5)
    DDRD |= (1 << PD6);     // PB1 is OUTPUT (pin 6 )   
    
    // TCCR0A |= (1 << WGM01) | (1 << WGM00);   // Fast PWM
    TCCR0A |= (1 << WGM00);   // PWM Phase Corrected
    TCCR0A |= (1 << COM0B1);  // Fast mode Pin 5
    TCCR0A |= (1 << COM0A1);  // Fast mode Pin 6
    TCCR0A |= (1 << COM0A1);  // None-inverted mode Pin 6
    TCCR0A |= (1 << COM0B1);  // None-inverted mode Pin 5
    // TCCR0A |= (1 << COM0B1) | (1 << COM0B0);  // inverted mode Pin 5

    TCCR0B |= (1 << CS10);   // No Prescaling = F_Clock or F_clock/1=16mhz
    // TCCR0B = TCCR0B & B11111000 | B00000001; // for PWM frequency of 62500.00 Hz
    
    
    // PWM by timer 2 and Interrupt ---------------------------------------------------
    TCNT2 = 0;
    TCCR2A = 0; TCCR2B = 0;
    DDRD |= (1 << PD3);   // output Pin 3
    DDRB |= (1 << PB3);   // output Pin 11
    TCCR2A |= (1 << COM2A1);    // None-inverted Pin 11
    TCCR2A |= (1 << COM2B1) | (1 << COM2B0);    // inverted Pin 3
    // TCCR2A |= (1 << WGM21) | (1 << WGM20);      // Fast PWM
    TCCR2A |= (1 << WGM20);      // PWM Phase Corrected
    TCCR2B |= (0 << CS22) | (0 << CS21) | (1 << CS20);  // Prescaler
    // OCR2A = TOP2 - 1;              // Frequence = (16000000/((TOP2 + 1) * Prescaler))
    
    //TIMSK2 |= (1 << OCIE2A);  // Output Compare Match A Interrupt Enable
                                // TIMER2_COMPA_vect
            
    // TIMSK2 |= (1 << TOIE2); // Timer2 Overflow Interrupt Enable
    // sei();                  // Enable global interrupts   


    // PWM by timer 1 & Interrupt-------------------------------------------------
    TCNT1 = 0;
    TCCR1A = 0; TCCR1B = 0; // Reset 2 registers
    DDRB |= (1 << PB2);     // PB2 is OUTPUT (pin 10)
    DDRB |= (1 << PB1);     // PB1 is OUTPUT (pin 9 )   
    
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12)|(1 << WGM13); // select Fast PWM mode select TOP_value freely ICR1                     
    TCCR1A |= (1 << COM1B1); // None-inverted mode
    TCCR1A |= (1 << COM1A1);    // None-inverted mode Pin 9
    // TCCR1A |= (1 << COM1B1) | (1 << COM1B0);  // inverted mode Pin 10
    
    ICR1 = 3199;                // Frequency = 16M / ICR1
    TCCR1B |= (1 << CS10);              // No Prescaling = F_Clock or F_clock/1=16mhz
    
    TIMSK1 |= (1 << TOIE1);         // Timer1 Overflow Interrupt Enable
    sei();
}

void loop() {

}
