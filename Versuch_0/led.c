/*
 * led.c
 *
 * Created: 22.10.2026 14:37:49
 *  Author: QP
 */ 

 #include "button.h"
 #include <avr/io.h>


void led_init(void) {
	DDRD |= 0b11111111;
	PORTD |= 0b11111111;
}

void led_fun(void) {
	int flag = 1;
	while (flag)
	{
		uint8_t buttonState = getInput();
		
		
		//Taster 1
		if ((buttonState & 1) == 1)
		{
			waitForNoInput();
			PORTD ^= 0b00000001;
		}
		
		//Taster 2
		if ((buttonState & 0b00000010) == 0b00000010)
		{
			waitForNoInput();
			if ((PORTD & 0b10000000) == 0b10000000)
			{
				PORTD = PORTD << 1;
				PORTD |= 0b00000001;
			} else {
				PORTD = PORTD << 1;
			}
		}
		
		//Taster 3
		if ((buttonState & 0b00000100) == 0b00000100)
		{
			waitForNoInput();
			PORTD = ~PORTD;
		}
	}
}