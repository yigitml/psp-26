#include <avr/io.h>

#include "os_input.h"
#include "manuell.h"
#include "sar.h"
#include "tracking.h"



int main(void) {
	switch (1)
	{
	case 1:
		manuell();
	
	case 2:
		sar();
		
	case 3:
		tracking();
	}
}
