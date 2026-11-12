#include "led.h"

#include <avr/io.h>

uint16_t activateLedMask = 0xFFFF;


/*!
 *  Initializes the led bar. Note: All PORTs will be set to output.
 */
void initLedBar(void) {
	// 1111_1111_xxxx_xxxx >> 8
	DDRD |= (activateLedMask >> 8);
	// xxxx_xxxx_1111_1111
	DDRA |= ((uint8_t) activateLedMask);
}

/*!
 *  Sets the passed value as states of the led bar (1 = on, 0 = off).
 */
void setLedBar(uint16_t value) {
	/* Change code to this if A8 doesnt work
	uint8_t maskD = (uint8_t)(activateLedMask >> 8);

	PORTA = (PORTA & (uint8_t) ~activateLedMask) | ((uint8_t) ~value & (uint8_t) activateLedMask);
	PORTD = (PORTD & ~maskD | ((uint8_t)~(value >> 8) & maskD);
	*/
	
	// 1111_1111_xxxx_xxxx >> 8
	PORTD = ~(value >> 8);
	// xxxx_xxxx_1111_1111
	PORTA = ~((uint8_t) value);
}
