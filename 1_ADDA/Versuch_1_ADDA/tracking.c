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
	// konfigurieren Sie zunachst Port C0 (Komparator) als Eingang
	DDRC &= 0b11111110;
	// Pull-Up Widerstand aktivieren
	PORTC |= 0b00000001;
	
	// Port A (LEDs) und Port B (R-2R-Netzwerk) als Ausgang konfigurieren
	DDRA |= 0b11111111;
	DDRB |= 0b11111111;
	
	os_initInput();
	
	// Setze Refspannung *nur* am Anfang
	uint8_t ref = 0;
	
	PORTA = ~ref;
	PORTB = ref;
	_delay_ms(50);
	
	while (1)
	{
		os_waitForInput();
		os_waitForNoInput();
		// K = 1 also Ref > Mess: Ref dekrementieren bis Mess > Ref
		if (PINC & 1)
		{
			while (PINC & 1){
				if (ref > 0){ref--;}
				
				PORTA = ~ref;
				PORTB = ref;
				_delay_ms(50);
			}
			
		}
		// // K = 0 also Mess > Ref: Ref inkrementieren bis Ref > Mess
		else {
			while (!(PINC & 1)){
				if (ref < 255){ref++;}
				else {break;}
				
				PORTA = ~ref;
				PORTB = ref;
				_delay_ms(50);
			
			}
			
		}
	}
 }
 
 