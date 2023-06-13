#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define pi 3.1415926535897932384626433832795
#define F 50.0
#define M

volatile float data;
volatile float t = 0.0;

void sinWave(void){
    printf("int data[201] = {");
    for (int i = 0; i < 200; i++){                                          // interrupt 10 kHz - 200 sample
        data = (M * 256 * sin(2.0*pi*F*t) + 256.0) / 2.0;                   // Phase 0
        // data = (0.7*256 * sin(2.0*pi*F*t + 2.0*pi/3.0) + 256.0) / 2.0;   // Phase 1
        // data = (0.7*256 * sin(2.0*pi*F*t + 4.0*pi/3.0) + 256.0) / 2.0;   // Phase 2
        printf("%d, ", (int)round(data));        
        t += 0.0001; // 0 <= t <= 0.02
    }
    printf("}\n");
}


int main(int argc, char const *argv[]){

    sinWave();
    return 0;
}
