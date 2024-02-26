/**
 * Driver for ping sensor
 * @file ping.c
 * @author
 */

#include "ping.h"
#include "Timer.h"
#include "lcd.h"

volatile unsigned long START_TIME = 0;
volatile unsigned long END_TIME = 0;
volatile enum{LOW, HIGH, DONE} PING_STATE = LOW; // State of ping echo pulse

/**
 * Initialize ping sensor. Uses PB3 and Timer 3B
 */
void ping_init (void){
    SYSCTL_RCGCTIMER_R |= 0x08;
    SYSCTL_RCGCGPIO_R |= 0x02; // activate clock for Port B
    while((SYSCTL_PRGPIO_R&0x02) != 0x02){}; // for stabilization
    GPIO_PORTB_DIR_R |= 0x08; // make PB3 output
    GPIO_PORTB_DEN_R |= 0x08; // enable digital I/O on PB3
    GPIO_PORTB_AFSEL_R |= 0x08;
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~0xF000) | 0x7000;

    TIMER3_CTL_R &= ~0x100;
    TIMER3_CFG_R |= 0x4;
    TIMER3_TBMR_R = (TIMER3_TBMR_R & 0x1F) | 0x7;
    TIMER3_CTL_R |= 0x0C00;
    TIMER3_TBILR_R |= 0xFFFF;
    TIMER3_TBPR_R = 0xFF;
    TIMER3_ICR_R |= 0x400;
    TIMER3_IMR_R |= 0x400;
    NVIC_PRI9_R |= 0x20;
    NVIC_EN1_R |= 0x10;


    IntRegister(INT_TIMER3B, TIMER3B_Handler);

    IntMasterEnable();

    // Configure and enable the timer
    TIMER3_CTL_R |= 0x100;
}

/**
 * @brief Trigger the ping sensor
 */
void ping_trigger (void){
    PING_STATE = LOW;
    // Disable timer and disable timer interrupt
     TIMER3_CTL_R &= ~0x100;
     TIMER3_IMR_R &= ~0x400;
    // Disable alternate function (disconnect timer from port pin)
     GPIO_PORTB_AFSEL_R &= ~0x08;

    GPIO_PORTB_DATA_R &= ~0x8; // clear
    GPIO_PORTB_DATA_R |= 0x8; // start pulse
    timer_waitMicros(5);
    GPIO_PORTB_DATA_R &= ~0x8; // end pulse

    // Clear an interrupt that may have been erroneously triggered
     TIMER3_ICR_R |= 0x400;
    // Re-enable alternate function, timer interrupt, and timer
     GPIO_PORTB_AFSEL_R |= 0x08;
     TIMER3_IMR_R |= 0x400;
     TIMER3_CTL_R |= 0x100;
}

/**
 * @brief Timer3B ping ISR
 */
void TIMER3B_Handler(void){
    TIMER3_ICR_R |= 0x400;
    if (PING_STATE == LOW) {
        START_TIME = TIMER3_TBR_R;
        PING_STATE = HIGH;
    } else if (PING_STATE == HIGH) {
        END_TIME = TIMER3_TBV_R;
        PING_STATE = DONE;
    }
}

/**
 * @brief Calculate the distance in cm
 *
 * @return Distance in cm
 */
float ping_getDistance (void){
    ping_trigger();
    while (PING_STATE != DONE) {}
    uint16_t cycles;
    float milliseconds;
    float distcm;
    if (START_TIME > END_TIME) {
        cycles = START_TIME - END_TIME;
        milliseconds = cycles * (1.0 / 16000.0);
        distcm = ((milliseconds / 1000.0) * 34300.0) / 2.0;
    } else {
        cycles = (START_TIME - END_TIME) + 0xFFFFFF;
        milliseconds = cycles * (1.0 / 16000.0);
        distcm = ((milliseconds / 1000.0) * 34300.0) / 2.0;
    }
    return distcm;

}
