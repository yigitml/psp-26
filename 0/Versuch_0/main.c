#include <avr/io.h>
#include <stdint.h>

#include "button.h"
#include "led.h"
#include "lcd.h"
#include "datatypes.h"
#include "structures.h"
#include "pointer.h"


int main(void) {
	initInput();
	led_init();
	lcd_init();
	
	//Buttons test
	//buttonTest();
	
	//LEDS test
	//led_fun();
	
	//Datatypes test
	//loop();
	//convert();
	//shift();
	
	//Structures test
	//displayArticles();
	
	//Pointers test
	//learningPointers();
	
    /* Replace with your application code */
    while (1) {
		
    }
}
