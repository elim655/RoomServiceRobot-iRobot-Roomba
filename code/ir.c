/*
 * adc.c
 *
 *  Created on: Mar 23, 2023
 *      Author: jojoseph
 */

#include <ir.h>
#include "uart-interrupt.h"
#include <lcd.h>
#include <ping.h>
#include <math.h>

int initialized = 0;

volatile int firstRawIrValue = 1730;
volatile int secondRawIrValue = 1030;
int irSlope;
float intercept;

/*
 * Initializes the IR scanner
 */
void ir_init() {

    irSlope = (50 - 20) / ((1.0 / secondRawIrValue) - (1.0 / firstRawIrValue));
    intercept = 50.0 - irSlope * (1.0 / secondRawIrValue);

    SYSCTL_RCGCADC_R |= 0x0001; // 1) activate ADC0
    SYSCTL_RCGCGPIO_R |= 0x02; // 2) activate clock for Port B
    while((SYSCTL_PRGPIO_R&0x02) != 0x02){}; // 3 for stabilization
    GPIO_PORTB_DIR_R &= ~0x10; // 4) make PB4 input
    GPIO_PORTB_DEN_R &= ~0x10; // 5) disable digital I/O on PB4
    GPIO_PORTB_AMSEL_R |= 0x10; // 6) enable analog functionality on PB4
    while((SYSCTL_PRADC_R&0x01) != 0x01){}; // 7) Check if ADC0 is ready
    ADC0_ACTSS_R &= ~0x0008; // 10) disable sample sequencer 3
    ADC0_EMUX_R &= ~0xF000; // 11) seq3 is software trigger
    ADC0_SSMUX3_R &= ~0x000F;
    ADC0_SSMUX3_R |= 0x0A; // 12) set channel
    ADC0_SSCTL3_R = 0x0006; // 13) no TS0 D0, yes IE0 END0
    ADC0_ACTSS_R |= 0x0008; // 15) enable sample sequencer 3
    initialized = 1;
}

/*
 * scans the distance from an object using the IR sensor
 * and returns the raw conversion from voltage
 * return raw_value
 */
uint16_t ir_scan_raw() {
    if (initialized) {
        ADC0_PSSI_R = 0x0008; // 1) initiate SS3
        while((ADC0_RIS_R&0x08)==0){}; // 2) wait for conversion done
        uint16_t result = ADC0_SSFIFO3_R&0xFFF; // 3) read result
        ADC0_ISC_R = 0x0008; // 4) acknowledge completion
        int i;
        for(i = 0; i < 2; i++) {
            ADC0_PSSI_R = 0x0008; // 1) initiate SS3
            while((ADC0_RIS_R&0x08)==0){}; // 2) wait for conversion done
            result = (result + ADC0_SSFIFO3_R&0xFFF) / 2; // 3) read result
            ADC0_ISC_R = 0x0008; // 4) acknowledge completion
        }
        return result;
    }
    return 0;
}


/*
 * Calculates and returns the distance from an object
 * in cm
 * return distance_cm
 */
uint16_t ir_scan_cm() {
    if (!initialized) {
        return 0;
    }
    return irSlope * (1.0 / ir_scan_raw()) + intercept; //equation for conversion
}

/**
 * Calibrate IR with Ping distance value to calculate for slope and intercept for equation
 * get 2 raw value, 20cm and 50cm IR raw value
 */
void ir_calibrate() {
    if (!initialized)
    {
        return;
    }
    float distance = ping_getDistance();

    while (distance < 19 || distance > 21) { // looking for distance 20 centimeter
        lcd_printf("Distance: %f", distance);
        timer_waitMillis(500);
        distance = ping_getDistance();
        if (!calibrating) {
            return;
        }
    }
    firstRawIrValue = ir_scan_raw(); // get the raw value for 20 centimeter
    while (distance < 49 || distance > 51) { // looking for distance 50 centimeter
        lcd_printf("Distance: %f, First IR Raw: %d", distance, firstRawIrValue);
        timer_waitMillis(500);
        distance = ping_getDistance();
        if (!calibrating) {
            return;
        }
    }
    secondRawIrValue = ir_scan_raw(); // get the raw value for 50 centimeter
    lcd_printf("First (20cm): %d\nSeconds (50cm): %d\n", firstRawIrValue, secondRawIrValue); //print value on LCD
    char sendString[45];
    sprintf(sendString, "First (20cm): %d\nSeconds (50cm): %d\n", firstRawIrValue, secondRawIrValue); //send value back to computer through UART
    uart_print(sendString);

}


