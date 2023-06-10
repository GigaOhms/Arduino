#include <Arduino.h>
#include <avr/interrupt.h>
#include <math.h>

#define TOP0 256
#define TOP1 256
#define TOP2 256
#define M 0.7
#define F 50.0
#define SQRT3	1.7320508075688772935274463415059

// PWM 1-2 = PIN 5-6
// PWM 3-4 = PIN 9-10
// PWM 5-6 = PIN 3-11

// PWM pin 10 = 5 = 3
// PWM pin 11 = 9 = 6

volatile double t = 0.0;
volatile float va, vb, vc;
volatile int	phi, sect;
volatile double	T1, T2, T0;
volatile double	S1, S2, S3;
// volatile int TEST = 0;
// char buf[12];

void SVPWM(void);

ISR (TIMER2_OVF_vect){        //TIMER2_OVF_vect 
    TCNT0 = 0;
    TCNT1 = 0;
    TCNT2 = 0; 

    va = sin(2.0*PI*F*t);
    vb = sin(2.0*PI*F*t + 2.0*PI/3.0);
    vc = sin(2.0*PI*F*t + 4.0*PI/3.0);
    
    SVPWM();

    // Serial.println(S1 * 256);
    // Serial.println(data2);
    // Serial.println(data3);
    // f2str(data, buf);
    // Serial.println(buf);

    t += 0.000016;          // 1/62500
    if (t >= 0.019980){     // 1/Fsin - 1/62500 - 1/62500/4
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

    // PWM by timer 0 ---------------------------------------------------
    TCNT0 = 0;
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
    TCNT1 = 0;
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

void SVPWM(void){
    double	v_alpha	= 	sqrt(2.0 / 3.0) * (va - 0.5*vb - 0.5*vc);
    double	v_beta	=	sqrt(2.0 / 3.0) * SQRT3 / 2.0 * (vb - vc);
    double	theta	= 	atan2(v_beta, v_alpha) * 180/PI;

    if (theta > 0 && theta <= 60){
        phi	=	theta;
        sect	=	1;
    }
    else if (theta > 60 && theta <= 120){
        phi	=	theta - 60;
        sect	=	2;
    }
    else if (theta > 120 && theta <= 180){
        phi	=	theta - 120;
        sect	=	3;
    }
    else if (theta > -180 && theta <= -120){
        phi	=	theta + 180;
        sect	=	4;
    }
    else if (theta > -120 && theta <= -60){
        phi	=	theta + 120;
        sect	=	5;
    }
    else {
        phi	=	theta + 60;
        sect	=	6;
    }

    T1	=	M*sin((60 - phi) * PI /180);
    T2	=	M*sin(phi * PI/180);
    T0	=	1 - T1 - T2;

    switch (sect){
        case 1:
            S1	=	T1 + T2 + T0/2;
            S2	=	T2 + T0/2;
            S3	=	T0/2;
            break;

        case 2:
            S1	=	T1 + T0/2;
            S2	=	T1 + T2 + T0/2;
            S3	=	T0/2;
            break;

        case 3:
            S1	=	T0/2;
            S2	=	T1 + T2 + T0/2;
            S3	=	T2 + T0/2;
            break;

        case 4:
            S1	=	T0/2;
            S2	=	T1 + T0/2;
            S3	=	T1 + T2 + T0/2;
            break;

        case 5:
            S1	=	T2 + T0/2;
            S2	=	T0/2;
            S3	=	T1 + T2 + T0/2;
            break;

        case 6:
            S1	=	T1 + T2 + T0/2;
            S2	=	T0/2;
            S3	=	T1 + T0/2;
            break;
    }

    OCR0A = S1 * TOP0;           // PWM Pin 6 
    OCR0B = S1 * TOP0;           // PWM Pin 5 inverted
    OCR1A = S2 * TOP0;           // PWM Pin 9
    OCR1B = S2 * TOP0;           // PWM Pin 10 inverted
    OCR2A = S3 * TOP0;           // PWM Pin 11
    OCR2B = S3 * TOP0;           // PWM Pin 3 inverted
}
