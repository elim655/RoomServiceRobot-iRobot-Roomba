/*
 * movement.c
 *
 *  Created on: Feb 2, 2023
 *      Author: jojoseph
 */

#include "movement.h"

#define PI 3.14159265

volatile float xOffsetCm;
volatile float yOffsetCm;
volatile int angleOffset = 90;
volatile int moving = 0;

double move_forward(oi_t *sensor_data, double distance_mm) {
    moving = 1;
    char sendString[64];
    double distanceMoved = 0; // distance member in oi_t struct is type double
    double leftCorrection = 0;
    double rightCorrection = 0;
    while (distanceMoved < distance_mm) {
        oi_update(sensor_data);

         //Detects the hole and stops the wheels
         if((sensor_data ->cliffLeftSignal) < 500) {
             oi_setWheels(0,0);
             sprintf(sendString, "h(%.02f, %.02f, %d)", xOffsetCm, yOffsetCm, angleOffset);
             uart_graph(sendString);
             move_backward(sensor_data, 100);
             turn_right(sensor_data, 75);
             break;
         }
         else if((sensor_data ->cliffFrontLeftSignal) < 500) {
             oi_setWheels(0,0);
             sprintf(sendString, "h(%.02f, %.02f, %d)", xOffsetCm, yOffsetCm, angleOffset);
             uart_graph(sendString);
             move_backward(sensor_data, 300);
             turn_right(sensor_data, 90);
             break;
         }
         else if((sensor_data ->cliffFrontRightSignal) < 500) {
             oi_setWheels(0,0);
             sprintf(sendString, "h(%.02f, %.02f, %d)", xOffsetCm, yOffsetCm, angleOffset);
             uart_graph(sendString);
             move_backward(sensor_data, 100);
             turn_left(sensor_data, 90);
             break;
         }
         else if((sensor_data ->cliffRightSignal) < 500) {
             oi_setWheels(0,0);
             sprintf(sendString, "h(%.02f, %.02f, %d)", xOffsetCm, yOffsetCm, angleOffset);
             uart_graph(sendString);
             move_backward(sensor_data, 100);
             turn_right(sensor_data, 75);
             break;
         }

         //Detects the boundary
         else if((sensor_data ->cliffLeftSignal) > 2700) {
            oi_setWheels(0,0);
            sprintf(sendString, "t(%.02f, %.02f, %d)", xOffsetCm, yOffsetCm, angleOffset);
            uart_graph(sendString);
            sprintf(sendString, "Tape Detected");
            uart_print(sendString);
            move_backward(sensor_data, 100);
            distanceMoved -= 100;
            break;
         }
         else if((sensor_data ->cliffFrontLeftSignal) > 2700) {
            oi_setWheels(0,0);
            sprintf(sendString, "t(%.02f, %.02f, %d)", xOffsetCm, yOffsetCm, angleOffset);
            uart_graph(sendString);
            sprintf(sendString, "Tape Detected");
            uart_print(sendString);
            move_backward(sensor_data, 100);
            distanceMoved -= 100;
            break;
         }
         else if((sensor_data ->cliffFrontRightSignal) > 2700) {
             oi_setWheels(0,0);
             sprintf(sendString, "t(%.02f, %.02f, %d)", xOffsetCm, yOffsetCm, angleOffset);
             uart_graph(sendString);
             sprintf(sendString, "Tape Detected");
             uart_print(sendString);
             move_backward(sensor_data, 100);
             distanceMoved -= 100;
             break;
         }
         else if((sensor_data ->cliffRightSignal) > 2700) {
             oi_setWheels(0,0);
             sprintf(sendString, "t(%.02f, %.02f, %d)", xOffsetCm, yOffsetCm, angleOffset);
             uart_graph(sendString);
             sprintf(sendString, "Tape Detected");
             uart_print(sendString);
             move_backward(sensor_data, 100);
             break;
         }

        else if(sensor_data -> bumpLeft){
             oi_setWheels(0,0);
             sprintf(sendString, "b(%.02f, %.02f, %d)", xOffsetCm, yOffsetCm, angleOffset);
             uart_graph(sendString);
             move_backward(sensor_data, 100);
             distanceMoved -= 100;
             turn_right(sensor_data, 90);
             move_forward(sensor_data, 100);
             rightCorrection += 100;
             turn_left(sensor_data, 90);
         } else if(sensor_data -> bumpRight){
             oi_setWheels(0,0);
             sprintf(sendString, "b(%.02f, %.02f, %d)", xOffsetCm, yOffsetCm, angleOffset);
             uart_graph(sendString);
             move_backward(sensor_data, 100);
             distanceMoved -= 100;
             turn_left(sensor_data, 90);
             move_forward(sensor_data, 100);
             leftCorrection += 100;
             turn_right(sensor_data, 90);
           }

          else {
             oi_setWheels(100,100);
             distanceMoved += sensor_data -> distance;
        }
        xOffsetCm += (sensor_data -> distance / 10.0) * cos(PI * angleOffset / 180.0);
        yOffsetCm += (sensor_data -> distance / 10.0) * sin(PI * angleOffset / 180.0);
    }
    oi_setWheels(0,0); //stop
    oi_update(sensor_data);
    distanceMoved += sensor_data -> distance;
    xOffsetCm += (sensor_data -> distance / 10.0) * cos(PI * angleOffset / 180.0);
    yOffsetCm += (sensor_data -> distance / 10.0) * sin(PI * angleOffset / 180.0);
    sprintf(sendString, "m(%.02f, %.02f, %d)", xOffsetCm, yOffsetCm, angleOffset);
    uart_graph(sendString);
    sprintf(sendString, "Bot position\n x: %.02f\n y: %.02f\n Angle: %d", xOffsetCm, yOffsetCm, angleOffset);
    uart_print(sendString);
    moving = 0;
    return distance_mm;
}

double move_backward(oi_t *sensor_data, double distance_mm) {
    char sendString[64];
     double distanceMoved = 0; // distance member in oi_t struct is type double
     distance_mm = -1 * distance_mm;
     oi_setWheels(-100,-100); //move forward at full speed
     while (distanceMoved > distance_mm) {
       oi_update(sensor_data);
       distanceMoved += sensor_data -> distance; // use -> notation since pointer
       xOffsetCm += (sensor_data -> distance / 10.0) * cos(PI * angleOffset / 180.0);
       yOffsetCm += (sensor_data -> distance / 10.0) * sin(PI * angleOffset / 180.0);
     }
     oi_setWheels(0,0); //stop
     oi_update(sensor_data);
     distanceMoved += sensor_data -> distance; // use -> notation since pointer
     xOffsetCm += (sensor_data -> distance / 10.0) * cos(PI * angleOffset / 180.0);
     yOffsetCm += (sensor_data -> distance / 10.0) * sin(PI * angleOffset / 180.0);
     sprintf(sendString, "m(%.02f, %.02f, %d)", xOffsetCm, yOffsetCm, angleOffset);
     uart_graph(sendString);
     sprintf(sendString, "Bot position\n x: %.02f\n y: %.02f\n Angle: %d", xOffsetCm, yOffsetCm, angleOffset);
     uart_print(sendString);
     return distance_mm;
}

// keep degrees positive
double turn_right(oi_t *sensor, double degrees){
    char sendString[64];
    double degreesMoved = 0; // distance member in oi_t struct is type double
    oi_setWheels(-50,50); //move left wheel forward at half speed
    while (degreesMoved > degrees * -1) {
        oi_update(sensor);
        degreesMoved += sensor -> angle; // use -> notation since pointer
    }
    oi_setWheels(0,0); //stop
    oi_update(sensor);
    degreesMoved += sensor -> angle; // use -> notation since pointer
    angleOffset += degreesMoved;
    sprintf(sendString, "m(%.02f, %.02f, %d)", xOffsetCm, yOffsetCm, angleOffset);
    uart_graph(sendString);
    sprintf(sendString, "Bot position\n x: %.02f\n y: %.02f\n Angle: %d", xOffsetCm, yOffsetCm, angleOffset);
    uart_print(sendString);
    return degrees;
}

double turn_left(oi_t *sensor, double degrees){
    char sendString[64];
    double degreesMoved = 0; // distance member in oi_t struct is type double
    oi_setWheels(50,-50); //move right wheel forward at half speed
    while (degreesMoved < degrees) {
        oi_update(sensor);
        degreesMoved += sensor -> angle; // use -> notation since pointer
    }
    oi_setWheels(0,0); //stop
    oi_update(sensor);
    degreesMoved += sensor -> angle; // use -> notation since pointer
    angleOffset += degreesMoved;
    sprintf(sendString, "m(%.02f, %.02f, %d)", xOffsetCm, yOffsetCm, angleOffset);
    uart_graph(sendString);
    sprintf(sendString, "Bot position\n x: %.02f\n y: %.02f\n Angle: %d", xOffsetCm, yOffsetCm, angleOffset);
    uart_print(sendString);
    return degrees;
}

void cliff_calibrate(oi_t *sensor) {
    while (1) {
        oi_update(sensor);
        lcd_printf("%u", sensor ->cliffFrontLeftSignal);
        timer_waitMillis(200);
    }
}
