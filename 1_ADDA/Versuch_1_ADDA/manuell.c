/*
 * manuell.c
 *
 * Created: 3.11.2026 16:40:24
 *  Author: QP
 */ 

#include "manuell.h"

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "os_input.h"

void manuell(void) {
	// konfigurieren Sie zunächst Port D (DIP-Schalter) als Eingang
	DDRD &= 0b00000000;
	PORTD |= 0b11111111;
	
	// Port A (LEDs) und Port B (R-2R-Netzwerk) als Ausgang konfiguriert werden
	DDRA |= 0b11111111;
	DDRB |= 0b11111111;
	
	while (1)
	{
		_delay_ms(50);
		PORTA = PIND;
		PORTB = ~PIND;
	}

}


