/**
 * Driver for the servo
 * @file servo.c
 * @author
 */

#include <ir.h>
#include "servo.h"
#include "Timer.h"
#include "lcd.h"
#include "uart-interrupt.h"
#include "button.h"

volatile enum{LOW, HIGH} SERVO_STATE = LOW; // State of ping echo pulse
volatile long matchRight = 312423;
volatile long matchLeft = 284736;
float servoSlope = -153.82;

/**
 * Initialize servo sensor. Uses PB5 and Timer 1B
 */
void servo_init (void){
    // Calibrate
    servoSlope = (matchRight - matchLeft) / -180;

    SYSCTL_RCGCTIMER_R |= 0x02;
    SYSCTL_RCGCGPIO_R |= 0x02; // activate clock for Port B
    while((SYSCTL_PRGPIO_R&0x02) != 0x02){}; // for stabilization
    GPIO_PORTB_DIR_R |= 0x20; // make PB5 output
    GPIO_PORTB_DEN_R |= 0x20; // enable digital I/O on PB5
    GPIO_PORTB_AFSEL_R |= 0x20; // Enable alternate function for PB5
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~0xF00000) | 0x700000; // Use alternate function 7 on PB5

    TIMER1_CTL_R &= ~0x100; // Turn off timer 1
    TIMER1_CFG_R = 0x4; // Use 16 bit for 16/32 or 32 bit for 32/64
    TIMER1_TBMR_R = 0xA; // Match interrupt enable, timer counts down, PWM enabled, periodic timer mode
    TIMER1_TBILR_R = 0xE200; // 20 millis of cycles is 0x4E200, ILR:0xE200, PR:0x04
    TIMER1_TBPR_R = 0x04;
    TIMER1_TBMATCHR_R = ((matchRight + matchLeft) / 2) & 0xFFFF; // 1.5 millis pulse for 90 deg. Match preset to 0x4A380
    TIMER1_TBPMR_R = ((matchRight + matchLeft) / 2) >> 16;
    NVIC_PRI5_R |= 0x200000;
    NVIC_EN0_R |= 0x400000;

    // Configure and enable the timer
    TIMER1_CTL_R |= 0x100;
}

/**
 * @brief Move the servo a specified number of degrees
 */
unsigned long servo_move(int degrees) {
    unsigned long matchValue = servoSlope * degrees + matchRight;
    TIMER1_CTL_R &= ~0x100;
    TIMER1_TBMATCHR_R = matchValue & 0xFFFF;
    TIMER1_TBPMR_R = matchValue >> 16;
    TIMER1_CTL_R |= 0x100;
    timer_waitMillis(100);
    return matchValue;
}


/*
 * Calibrate the servo by getting 0 degrees and 180 degrees of match value, calculate the slop and intercept for equation.
 */
void servo_calibrate(void) {
    int degrees = 90;
    int button;
    int right = 1;
    char sendString[30];
    unsigned long matchValue = servo_move(90);
    while(1) {
        button = button_getButton();
        if(button == 1) {
            degrees += 2;
            matchValue = servo_move(degrees);
        } else if (button == 2) {
            degrees -= 2;
            matchValue = servo_move(degrees);
        } else if (button == 4) {
            if (right) {
                matchRight = matchValue;
                right = 0;
                matchValue = servo_move(90);
                degrees = 90;
            } else {
                matchLeft = matchValue;
                break;
            }
            timer_waitMillis(200);
        }
        if (right) {
            lcd_printf("b1 for left, b2 for right\n0 MV: %u", matchValue);
        } else {
            lcd_printf("b1 for left, b2 for right\n0 MV: %u\n180 MV: %u", matchRight, matchValue);
        }
        if (!calibrating) {
            return;
        }
    }
    lcd_printf("0 MV: %u\n180 MV: %u", matchRight, matchLeft);
    sprintf(sendString, "0 MV: %u\n180 MV: %u", matchRight, matchLeft);
    uart_print(sendString);
}
