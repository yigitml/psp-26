#include <avr/io.h>
#include <stdint.h>

#include "button.h"
#include "led.h"
#include "lcd.h"
#include "datatypes.h"
#include "pointer.h"
#include "structures.h"


int main(void) {
	initInput();
	buttonTest();
	
	led_init();
	led_fun();,
	
	lcd_init();
	
	loop();
	convert();
	shift();
	
	
    /* Replace with your application code */
    while (1) {
		
    }
}
