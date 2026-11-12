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
	// konfigurieren Sie zunachst Port C0 (Komparator) als Eingang
	DDRC &= 0b11111110;
	// Pull-Up Widerstand aktivieren
	PORTC |= 0b00000001;
	
	// Port A (LEDs) und Port B (R-2R-Netzwerk) als Ausgang konfigurieren
	DDRA |= 0b11111111;
	DDRB |= 0b11111111;
	os_initInput();
	
	// Refspannung immer 0 am Anfang
	uint8_t ref = 0;
	PORTA = ~ref;
	PORTB = ref;
	// Delay wegen hohen Ausfuhrungsgeschwindigkeit des Mikrocontrollers
	_delay_ms(50);
	
	while (1)
	{	

		// Warte zuerst fur Input
		os_waitForInput();
		os_waitForNoInput();
		// Refspannung immer 0 am Anfang
		ref = 0;

		// Muss invertieren fur LEDs
		PORTA = ~ref;
		PORTB = ref;
		// Delay wegen hohen Ausfuhrungsgeschwindigkeit des Mikrocontrollers
		_delay_ms(50);
		
		// Starte mit linkstem Bit (MSB) und gehe bis fertig mit rechstem Bit (LSB)
		for (int i = 0; i <= 7; i++)
		{
			// Setze immer das relevante Bit 1.
			ref |= (1 << (7-i));
			PORTA = ~ref;
			PORTB = ref;
			_delay_ms(50);
			
			// Falls K = 0: Setze das Bit als 0 zuruck.
			if ((PINC & 1))
			{
				ref &= ~(1 << (7-i));
				PORTA = ~ref;
				PORTB = ref;
				_delay_ms(50);
			}
		}
	}
}