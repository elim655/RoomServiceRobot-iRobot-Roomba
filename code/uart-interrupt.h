/*
*
*   uart-interrupt.h
*
*   Used to set up the RS232 connector and WIFI module
*   Uses RX interrupt
*   Functions for communicating between CyBot and PC via UART1
*   Serial parameters: Baud = 115200, 8 data bits, 1 stop bit,
*   no parity, no flow control on COM1, FIFOs disabled on UART1
*
*   @author Dane Larson
*   @date 07/18/2016
*   Phillip Jones updated 9/2019, removed WiFi.h, Timer.h
*   Diane Rover updated 2/2020, added interrupt code
*/

#ifndef UART_H_
#define UART_H_

#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/interrupt.h"

// Notice that interrupt.h provides library function prototypes for IntMasterEnable() and IntRegister()

// The following externals are global variables defined in uart-interrupt.c for use with the interrupt handler.
// Using extern here, the global variables become visible to other c files that include uart-interrupt.h
// Extern does not allocate storage for a variable. It tells the compiler that the variable is defined in another file.
//extern volatile char receive_buffer[]; // buffer for characters received from PuTTY
//extern volatile int receive_index; // index to keep track of characters in buffer
extern volatile char scan_command_byte;
extern volatile char stop_command_byte;
extern volatile char calibrate_command_byte;
extern volatile char move_forward_command_byte;
extern volatile char turn_left_command_byte;
extern volatile char turn_right_command_byte;
extern volatile int scan_enabled;
extern volatile int calibrating;
extern volatile int move_f;
extern volatile int move_b;
extern volatile int turn_l;
extern volatile int turn_r;

// UART1 device initialization for CyBot to PuTTY
void uart_interrupt_init(void);

// Send a byte over UART1 from CyBot to PuTTY
void uart_sendChar(char data);

// CyBot waits (i.e. blocks) to receive a byte from PuTTY
// returns byte that was received by UART1
// Not used with interrupts; see UART1_Handler
char uart_receive(void);

// Send a string over UART1
// Sends each char in the string one at a time
void uart_print(const char *data);


/*
 * Send a string over UART1 for graphing
 * Sends each char in the string one at a time
 */
void uart_graph(const char *data);

// Interrupt handler for receive interrupts
void UART1_Handler(void);

#endif /* UART_H_ */
