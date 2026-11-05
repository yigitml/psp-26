#include "os_input.h"

#include <avr/io.h>
#include <stdint.h>

/*! \file

Everything that is necessary to get the input from the Buttons in a clean format.

*/

/*!
 *  A simple "Getter"-Function for the Buttons on the evaluation board.\n
 *
 *  \returns The state of the button(s) in the lower bits of the return value.\n
 *  example: 1 Button:  -pushed:   00000001 C11000011
 *                      -released: 00000000
 *           4 Buttons: 1,3,4 -pushed: 000001101
 *
 */
uint8_t os_getInput(void) {
	uint8_t buttons12 = (PINC & 0b00000011);
	uint8_t buttons34 = (PINC & 0b11000000);
	buttons34 = buttons34 >> 5;
	
	uint8_t buttons = buttons12 | buttons34;
	return (0b00001111 & ~buttons); 
}

/*!
 *  Initializes DDR and PORT for input
 */
void os_initInput() {
	DDRC &= 0b00111100;
	PORTC |= 0b11000011;
}

/*!
 *  Endless loop as long as at least one button is pressed.
 */
void os_waitForNoInput() {
	while (os_getInput() != 0){}
}

/*!
 *  Endless loop until at least one button is pressed.
 */
void os_waitForInput() {
	while (os_getInput() == 0){}
}
