/*
 * sar.c
 *
 * Created: 3.11.2026 16:57:44
 *  Author: QP
 */ 

#include "sar.h"

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "os_input.h"

void sar(void){
	DDRC &= 0b11111110;
	PORTC |= 0b00000001;
	
	DDRA |= 0b11111111;
	DDRB |= 0b11111111;
	os_initInput();
	
	while (1)
	{
		os_waitForInput();
		os_waitForNoInput();
		uint8_t ref = 0b00000000;
		_delay_ms(50);
		PORTA = ~ref;
		PORTB = ref;
		
		for (int i = 0; i < 7; i++)
		{
			ref |= (1 << (7-i));
			_delay_ms(50);
			PORTA = ~ref;
			PORTB = ref;
			
			if ((PINC & 1))
			{
				ref &= ~(1 << (7-i));
				_delay_ms(50);
				PORTA = ~ref;
				PORTB = ref;
			}
		}
	}
}