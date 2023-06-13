#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define pi 3.1415926535897932384626433832795
#define F 50.0
#define togiGain 1.0f
#define togiWTz 3.1415926535897932384626433832795e-2f
#define M 0.7
#define SQRT3	1.7320508075688772935274463415059

volatile float va, vb, vc;
volatile float t = 0.0;
volatile int	phi, sect;
volatile double	T1, T2, T0;
volatile double	S1, S2, S3;

void SVPWM(void);
void SVWave(void);

int main(int argc, char const *argv[])
{
    SVWave();
    return 0;
}

void SVWave(void)
{
    printf("int data[201] = {");
    for (int i = 0; i < 200; i++){
        va = (sin(2.0*pi*F*t) + 1) / 2.0;
        vb = (sin(2.0*pi*F*t + 2.0*pi/3.0) + 1.0) / 2.0;
        vc = (sin(2.0*pi*F*t + 4.0*pi/3.0) + 1.0) / 2.0;
        SVPWM();
        printf("%d, ", (int)round(S1 * 255));        
        t += 0.0001; // 0 <= t <= 0.02
    }
    printf("}\n");
}

void SVPWM(void)
{
    double	v_alpha	= 	sqrt(2.0 / 3.0) * (va - 0.5*vb - 0.5*vc);
    double	v_beta	=	sqrt(2.0 / 3.0) * SQRT3 / 2.0 * (vb - vc);
    double	theta	= 	atan2(v_beta, v_alpha) * 180/pi;

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

    T1	=	M * sin((60 - phi) * pi /180);
    T2	=	M * sin(phi * pi/180);
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

}
