/**
 * @file lab9_template.c
 * @author
 * Template file for CprE 288 Lab 9
 */

#include "button.h"
#include "ir.h"
#include "lcd.h"
#include "movement.h"
#include "object.h"
#include "open_interface.h"
#include "ping.h"
#include "servo.h"
#include "Timer.h"
#include "uart-interrupt.h"
#include <inc/tm4c123gh6pm.h>
#include <math.h>

#define PI 3.14159265

int main(void) {

    // Callibration values
     matchRight = 312621;
     matchLeft = 284721;

    firstRawIrValue = 1697;
    secondRawIrValue = 1006;

    // Local variables
    int i;
    char sendString[75]; // Intermediary string to hold formated strings before sending
    uint16_t irDistanceCm;
    float pingDistanceCm;
    float closestObject = 101;
    float x;
    float y;

    // Initialization
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
	timer_init(); // Must be called before lcd_init(), which uses timer functions
	lcd_init();
	ping_init();
	servo_init();
	button_init();
	ir_init();
	uart_interrupt_init();

	uart_print("Hello, World!\n");
	while (1) {
	    if (calibrating) {
            servo_calibrate();
            servo_move(90);
	        ir_calibrate();
	        cliff_calibrate(sensor_data);
	        calibrating = 0;
	    }
	    else if (scan_enabled) { // Wait for scan command
	        closestObject = 101;
            for (i = 45; i < 135 && scan_enabled; i += 2) {
                servo_move(i);
                irDistanceCm = ir_scan_cm();
                // Scan for reflective tape, print door found, print object width

                if (irDistanceCm < 40){
                    pingDistanceCm = ping_getDistance();
                    x = pingDistanceCm * cos(PI * (i + angleOffset - 90) / 180.0) + xOffsetCm;
                    y = pingDistanceCm * sin(PI * (i + angleOffset - 90) / 180.0) + yOffsetCm;
                    sprintf(sendString, "(%.02f, %.02f, %d)", x, y, angleOffset);
                    uart_graph(sendString);
                    if (pingDistanceCm < closestObject) {
                        closestObject = pingDistanceCm;
                    }
                }
            }
            scan_enabled = 0;
            sprintf(sendString, "Done Scanning");
            uart_print(sendString);
        }
	    else if (move_f) {
	        if (closestObject > 100) {
	            move_forward(sensor_data, 100);
	        } else {
	            move_forward(sensor_data, closestObject);
	        }
	        move_f = 0;
	    }
	    else if (move_b) {
            move_backward(sensor_data, 100);
            move_b = 0;
        }
	    else if (turn_l) {
	        turn_left(sensor_data, 45);
	        turn_l = 0;
	    }
	    else if (turn_r) {
	        turn_right(sensor_data, 45);
	        turn_r = 0;
	    }
	}
}
