#include "bin_clock.h"

#include <avr/interrupt.h>
#include <avr/io.h>

//! Global variables
uint16_t millisekunde;
uint8_t sekunde;
uint8_t minute;
uint8_t stunde;

/*!
 * \return The milliseconds counter of the current time.
 */
uint16_t getTimeMilliseconds() {
    return millisekunde;
}

/*!
 * \return The seconds counter of the current time.
 */
uint8_t getTimeSeconds() {
    return sekunde;
}

/*!
 * \return The minutes counter of the current time.
 */
uint8_t getTimeMinutes() {
    return minute;
}

/*!
 * \return The hour counter of the current time.
 */
uint8_t getTimeHours() {
    return stunde;
}

/*!
 *  Initializes the binary clock (ISR and global variables)
 */
void initClock(void) {
    // Set timer mode to CTC
    TCCR0A &= ~(1 << WGM00);
    TCCR0A |= (1 << WGM01);
    TCCR0B &= ~(1 << WGM02);

    // Set prescaler to 1024
    TCCR0B |= (1 << CS02) | (1 << CS00);
    TCCR0B &= ~(1 << CS01);

    // Set compare register to 195 -> match every 10ms
    OCR0A = 195;

// Init variables
	millisekunde = 0;
	sekunde = 45;
	minute = 59;
	stunde = 12;

    // Enable timer and global interrupts
    TIMSK0 |= (1 << OCIE0A);
    sei();
}

/*!
 *  Updates the global variables to get a valid 12h-time
 */
void updateClock(void){
	if (millisekunde >= 1000){
		sekunde++;
		millisekunde = 0;
	}
	
	if (sekunde >= 60){
		minute++;
		sekunde = 0;
	}
	
	if (minute >= 60){
		stunde++;
		minute = 0;
	}
	
	if (stunde >= 13){
		stunde = 1;
	}
}

/*!
 *  ISR to increase millisecond-counter of the clock
 */
ISR(TIMER0_COMPA_vect) {
	millisekunde += 10;
	updateClock();
}
