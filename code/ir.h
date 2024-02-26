/*
 * adc.h
 *
 *  Created on: Mar 23, 2023
 *      Author: jojoseph
 */

#ifndef IR_H_
#define IR_H_

#include <stdint.h>
#include <inc/tm4c123gh6pm.h>

extern volatile int firstRawIrValue;
extern volatile int secondRawIrValue;

/*
 * Initializes the IR scanner
 */
void ir_init(void);

/*
 * scans the distance from an object using the IR sensor
 * and returns the raw conversion from voltage
 * return raw_value
 */
uint16_t ir_scan_raw(void);

/*
 * Calculates and returns the distance from an object
 * in cm
 * return distance_cm
 */
uint16_t ir_scan_cm(void);

/**
 * Calibrate IR with Ping distance value to calculate for slope and intercept for equation
 * get 2 raw value, 20cm and 50cm IR raw value
 */
void ir_calibrate(void);

#endif /* IR_H_ */
