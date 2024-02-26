/*
 * movement.h
 *
 *  Created on: Feb 2, 2023
 *      Author: jojoseph
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "open_interface.h"
#include "lcd.h"
#include "uart-interrupt.h"
#include "Timer.h"
#include "lcd.h"
#include <math.h>
#include <stdio.h>


extern volatile float xOffsetCm;
extern volatile float yOffsetCm;
extern volatile int angleOffset;


/*
 * move the bot forward
 * sensor data - reallocate variable from open interface
 * distance_mm - distance to move in millimeter
 */
double move_forward(oi_t *sensor_data, double distance_mm);

/*
 * move the bot backward
 * sensor data - reallocate variable from open interface
 * distance_mm - distance to move in millimeter
 */
double move_backward(oi_t *sensor_data, double distance_mm);

/*
 * turn the bot right
 * sensor data - reallocate variable from open interface
 * degrees - degrees to turn
 */
double turn_right(oi_t *sensor, double degrees);

/*
 * turn the bot left
 * sensor data - reallocate variable from open interface
 * degrees - degrees to turn
 */
double turn_left(oi_t *sensor, double degrees);

void cliff_calibrate(oi_t *sensor);


#endif /* MOVEMENT_H_ */
