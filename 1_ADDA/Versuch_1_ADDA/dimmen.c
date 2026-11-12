/*
 * dimmen.c
 *
 * Created: 11.11.2026 12:35:48
 *  Author: jageqoxoso
 */ 


#include "dimmen.h"

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "os_input.h"
#include <stdio.h>

void dimmen(void) {
	
	os_initInput();
	
	// Port B (R-2R-Netzwerk) als Ausgang konfigurieren
	DDRB |= 0b11111111;
	
	
	uint8_t spannung = 0;
	
	while (1)
	{
		
		os_waitForInput();
		spannung++;
		
		if (spannung >= 255) {
			_delay_ms(100);
			spannung = 0;
		}
		// Delay wegen hohen Ausführungsgeschwindigkeit des Mikrocontrollers
		_delay_ms(50);
		// .
		PORTB = spannung;
	}
}