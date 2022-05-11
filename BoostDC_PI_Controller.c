#include <avr/interrupt.h>

#define kp 0.1
#define ki 0.01
#define Ts 0.001       // Sample time

double error;
double integral;
double D;
double voltFB;
double volt;

ISR (TIMER2_COMPA_vect){        //TIMER2_OVF_vect
    double setpoint = 12;
    double result;

    volt = analogRead(A3);
    voltFB = volt * 5 * 5 / 1023;

    error = setpoint - voltFB;
    integral = integral +  ki*error*Ts;

    result = kp*error + integral;

    if (result > 0.9)
        result = 0.9;
    if (result  < 0)
        result = 0;

    D = result;
    //digitalWrite(12,!digitalRead(12));
}

void setup(){ 
    Serial.begin(9600);

    // Timer 2 interrupt with sample time Ts = 0.001s (frequency 1000Hz) 
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;

    OCR2A = 15;               // ~ 1000 Hz = (16000000/((OCR2A + 1)*1024))
    TCCR2A |= (1 << WGM21);   // CTC
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);  // Prescaler 1024
    TIMSK2 |= (1 << OCIE2A);  // Output Compare Match A Interrupt Enable

    sei(); // Enable global interrupts   

    // PWM by timer 1 with frequency 20kHz
    TCCR1A = 0; TCCR1B = 0; // Reset 2 registers
    DDRB |= (1 << PB2); // PB2 is OUTPUT ( pin 10)                 
    
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12)|(1 << WGM13); // select Fast PWM mode select TOP_value freely ICR1                     
    TCCR1A |= (1 << COM1B1); // None-inverted mode
    ICR1 = 800;              // Sawtooth pulse overflow after 800 clocks or Top_value = 800
    TCCR1B |= (1 << CS10);   // No Prescaling = F_Clock or F_clock/1=16mhz
    //F_pwm = 16mhz/800 = 20kHz
}

void loop(){
    OCR1B = D*800;           // Duty cycle D%
    Serial.println(voltFB);
}
