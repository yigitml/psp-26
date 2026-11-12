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
	// konfigurieren Sie zunachst Port D (DIP-Schalter) als Eingang
	DDRD &= 0b00000000;
	// Pull-Up Widerstand aktivieren
	PORTD |= 0b11111111;
	
	// Port A (LEDs) und Port B (R-2R-Netzwerk) als Ausgang konfigurieren
	DDRA |= 0b11111111;
	DDRB |= 0b11111111;
	
	while (1)
	{
		// Delay wegen hohen Ausführungsgeschwindigkeit des Mikrocontrollers
		_delay_ms(50);
		// Muss nicht invertieren, LEDs leuchten bei LOW an
		PORTA = PIND;
		// Muss invertieren, da DIP-Schalter bei 0 ON sind.
		PORTB = ~PIND;
	}

}