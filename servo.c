#include <LPC21xx.h>
#include "header.h"

#define SERVO_PIN   (1 << 8)    // P0.8 / PWM4

void servo_init(void)
{
    PINSEL0 &= ~(3 << 16);
    PINSEL0 |=  (2 << 16);

    PWMPR = 29;
    PWMMR0 = 20000;
    PWMMR4 = 1000;
    PWMMCR = (1 << 1);
    PWMPCR = (1 << 12);

    PWMLER = (1 << 0) | (1 << 4);
    PWMTCR = (1 << 1);
    PWMTCR = (1 << 0);
}

void servo_0deg(void)
{
    PWMMR4 = 1000;       // 1 ms
    PWMLER = (1 << 4);
}

void servo_90deg(void)
{
    PWMMR4 = 1500;       // 1.5 ms
    PWMLER = (1 << 4);
}

void servo_180deg(void)
{
    PWMMR4 = 2000;       // 2 ms
    PWMLER = (1 << 4);
}