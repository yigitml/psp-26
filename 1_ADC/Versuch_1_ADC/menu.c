#include "menu.h"

#include "adc.h"
#include "bin_clock.h"
#include "lcd.h"
#include "led.h"
#include "os_input.h"

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

/*!
 *  Hello world program.
 *  Shows the string 'Hello World!' on the display.
 */
void helloWorld(void) {
// Repeat until ESC gets pressed
	while (1) {
		if (os_getInput() >> 3) {
			os_waitForNoInput();
			break;
		}
		lcd_writeString("Hallo Welt!");
		
		if (os_getInput() >> 3) {
			os_waitForNoInput();
			break;
		}
		_delay_ms(500);
		
		if (os_getInput() >> 3) {
			os_waitForNoInput();
			break;
		}
		lcd_clear();
		
		if (os_getInput() >> 3) {
			os_waitForNoInput();
			break;
		}
		_delay_ms(500);

	}
}

/*!
 *  Shows the clock on the display and a binary clock on the led bar.
 */
void displayClock(void) {
	while (1){
		if (os_getInput() >> 3){
			os_waitForNoInput();
			break;
		}
		
		// Reset LCD
		_delay_ms(50);
		lcd_clear();
		
		uint16_t millisekunde = getTimeMilliseconds(); 
		uint8_t sekunde = getTimeSeconds();
		uint8_t minute = getTimeMinutes(); // xxxx xxxx xx11 1111 << 6 == 1111 xxxx xxxx xxxx
		uint8_t stunde = getTimeHours(); //  xxxx xxxx xxxx 1111 << 12 == 1111 xxxx xxxx xxxx

		// LED
		uint16_t clockVal  = ((uint16_t) stunde << 12) | ((uint16_t) minute << 6) | sekunde;
		setLedBar(clockVal);
		
		// LCD (12:59:45:000)
		if (stunde < 10){lcd_writeDec(0);}
		lcd_writeDec(stunde);
		
		if (minute < 10){lcd_writeDec(0);}
		lcd_writeDec(minute);		
		
		if (sekunde < 10){lcd_writeDec(0);}
		lcd_writeDec(sekunde);
		
		if (millisekunde < 100){lcd_writeDec(0);}
		if (millisekunde < 10){lcd_writeDec(0);}
		lcd_writeDec(millisekunde);
	}
}

/*!
 *  Shows the stored voltage values in the second line of the display.
 */
void displayVoltageBuffer(uint8_t displayIndex) {
	lcd_erase(2);
	lcd_line2();
	if (displayIndex < 100){lcd_writeDec(0);}
	if (displayIndex < 10){lcd_writeDec(0);}
	lcd_writeDec(displayIndex);
	lcd_writeString("/");
	lcd_writeDec(100);
	lcd_writeString(": ");
	uint16_t storedVoltage = getStoredVoltage(displayIndex - 1);
	lcd_writeVoltage(storedVoltage, 1023, 5);
}

/*!
 *  Shows the ADC value on the display and on the led bar.
 */
void displayAdc(void) {
	uint8_t zahlIndex = 1;
	lcd_erase(1);
	lcd_line1();
	displayVoltageBuffer(zahlIndex);
	
	while (1){
		if (os_getInput() >> 3){
			os_waitForNoInput();
			break;
		}
		_delay_ms(100);
		lcd_erase(1);
		lcd_line1();

		uint16_t ledValue = 0;
		uint16_t adcResult = getAdcValue();
	
		//LCD Line 1
		lcd_writeString("Voltage: ");
		lcd_writeVoltage(adcResult, 1023, 5);
	
		while (adcResult >= 68){
			ledValue = ledValue << 1;
			ledValue |= 2;
			adcResult -= 68;
		}
		setLedBar(ledValue);
		
		
		// Enter
		if (os_getInput() & 0b00000010){ 
			storeVoltage();
			displayVoltageBuffer(zahlIndex);
		}
		
		// Up
		if (os_getInput() & 2){
			if (zahlIndex < 100 && getBufferIndex() <= zahlIndex ){zahlIndex++;}
			displayVoltageBuffer(zahlIndex);
		}
		
		// Down
		if (os_getInput() & 4){
			if (zahlIndex > 1){zahlIndex--;}
			displayVoltageBuffer(zahlIndex);
		}
	}
}

/*! \brief Starts the passed program
 *
 * \param programIndex Index of the program to start.
 */
void start(uint8_t programIndex) {
    // Initialize and start the passed 'program'
    switch (programIndex) {
        case 0:
            lcd_clear();
            helloWorld();
            break;
        case 1:
            activateLedMask = 0xFFFF; // Use all LEDs
            initLedBar();
            initClock();
            displayClock();
            break;
        case 2:
            activateLedMask = 0xFFFE; // Don't use LED 0
            initLedBar();
            initAdc();
            displayAdc();
            break;
        default:
            break;
    }

    // Do not resume to the menu until all buttons are released
    os_waitForNoInput();
}

/*!
 *  Shows a user menu on the display which allows to start subprograms.
 */
void showMenu(void) {
    uint8_t pageIndex = 0;

    while (1) {
        lcd_clear();
        lcd_writeProgString(PSTR("Select:"));
        lcd_line2();

        switch (pageIndex) {
            case 0:
                lcd_writeProgString(PSTR("1: Hello world"));
                break;
            case 1:
                lcd_writeProgString(PSTR("2: Binary clock"));
                break;
            case 2:
                lcd_writeProgString(PSTR("3: Internal ADC"));
                break;
            default:
                lcd_writeProgString(PSTR("----------------"));
                break;
        }

        os_waitForInput();
        if (os_getInput() == 0b00000001) { // Enter
            os_waitForNoInput();
            start(pageIndex);
        } else if (os_getInput() == 0b00000100) { // Up
            os_waitForNoInput();
            pageIndex = (pageIndex + 1) % 3;
        } else if (os_getInput() == 0b00000010) { // Down
            os_waitForNoInput();
            if (pageIndex == 0) {
                pageIndex = 2;
            } else {
                pageIndex--;
            }
        }
    }
}
