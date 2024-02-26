/**
 * Driver for ping sensor
 * @file servo.c
 * @author
 */
#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

extern volatile long matchRight;
extern volatile long matchLeft;

/**
 * Initialize servo sensor. Uses PB5 and Timer 1B
 */
void servo_init (void);

/**
 * @brief Move the servo a specified number of degrees
 */
unsigned long servo_move(int degrees);

/*
 * Calibrate the servo by getting 0 degrees and 180 degrees of match value, calculate the slop and intercept for equation.
 */
void servo_calibrate(void);


#endif /* SERVO_H_ */
