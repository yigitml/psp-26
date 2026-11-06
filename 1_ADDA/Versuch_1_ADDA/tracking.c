/*
 * tracking.c
 *
 * Created: 3.11.2026 17:18:29
 *  Author: QP
 */ 

 #include "tracking.h"

 #include <avr/io.h>
 #include <stdint.h>
 #include <util/delay.h>
 #include "os_input.h"
 
void tracking(void){
	DDRC &= 0b11111100;
	PORTC |= 0b00000011;
	
	DDRA |= 0b11111111;
	DDRB |= 0b11111111;
	
	os_initInput();
	
	uint8_t ref = 0;
	_delay_ms(50);
	PORTA = ~ref;
	PORTB = ref;
	
	while (1)
	{
		os_waitForInput();
		os_waitForNoInput();
		if (PINC & 1)
		{
			while (PINC & 1){
				if (ref > 0){ref--;}
				_delay_ms(50);
				PORTA = ~ref;
				PORTB = ref;
			}
			
		}
		else {
			while (!(PINC & 1)){
				if (ref < 255){ref++;}
				_delay_ms(50);
				PORTA = ~ref;
				PORTB = ref;
			
			}
			
		}
	}
 }
 
 