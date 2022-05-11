#include <avr/interrupt.h>

#define kp 0.1
#define ki 0.01
#define Ts 0.001

double error;
double integral;
double D;

ISR (TIMER2_COMPA_vect){        //TIMER2_OVF_vect
    double setpoint = 12;
    double result;
    double voltFB;
    double volt;

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
    //digitalWrite(12,!digitalRead(12)); //Sáng tắt LED0
}


void setup(){ 
    //Serial.begin(9600);

    // Ngat Timer 2
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;

    OCR2A = 15;               // 1000.xxx Hz = (16000000/((OCR2A +1)*1024))
    TCCR2A |= (1 << WGM21);   // CTC
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);  // Prescaler 1024
    TIMSK2 |= (1 << OCIE2A);  // Output Compare Match A Interrupt Enable

    sei(); //Cho phép ngắt toàn cục   

    TCCR1A=0; TCCR1B=0; // RESET lại 2 thanh ghi
    DDRB |= (1 << PB2); // Đầu ra PB2 là OUTPUT ( pin 10)                 
    
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12)|(1 << WGM13); // chọn Fast PWM chế độ chọn TOP_value tự do  ICR1                     
    TCCR1A |= (1 << COM1B1); // So sánh thường( none-inverting)
    ICR1 = 800;              // xung răng cưa tràn sau 800 P_clock
    TCCR1B |= (1 << CS10);   // F_clock/1=16mhz
    //F_pwm = 16mhz/800 = 20kHz
}


void loop(){
    OCR1B = D*800;           // PWM D%
}
