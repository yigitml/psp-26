/*
 * datatypes.c
 *
 * Created: 22.10.2026 15:33:08
 *  Author: QP
 */ 

#include <stdint.h>
#include "button.h"
#include "lcd.h"

void loop(void) {
	uint8_t result = 0;
	for (uint8_t i = 5; i >= 0; i--) {
		result += i * 2 + 2;
	}
	lcd_writeProgString(PSTR("Loop finished: "));
	lcd_writeDec(result);
	waitForInput();
}

void convert(void) {
	uint8_t target = 200;
	uint16_t source = target + 98;
	target = source;
}

void shift(void) {
	uint32_t result = 1 << 31;
	lcd_write32bitHex(result);
}