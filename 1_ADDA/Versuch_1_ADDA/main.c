#include <avr/io.h>

#include "os_input.h"
#include "manuell.h"
#include "sar.h"
#include "tracking.h"
#include "dimmen.h"



int main(void) {
	// Switch case to pick between R2R (manuell), suksessive Approximation SAR, Tracking Wandler
	switch (4)
	{
		case 1:
		manuell();
		
		case 2:
		sar();
		
		case 3:
		tracking();
		
		case 4:
		dimmen();
	}
}
