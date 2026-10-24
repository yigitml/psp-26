/*
 * structures.c
 *
 * Created: 23.10.2026 15:07:59
 *  Author: QP
 */ 

#include "structures.h"
#include "lcd.h"
#include "button.h"
#include <stdint.h>
	

typedef enum {
	AVAILABLE,
	BACKORDER,
	SOLD_OUT = 99,
	} DistributionStatus;
	
typedef struct {
	uint8_t manufactureId;
	uint8_t productID;
	} ArticleNumber;
	
typedef union {
	uint16_t combinedNumber;
	ArticleNumber singleNumbers;
	} FullArticleNumber;
	
struct Article {
	FullArticleNumber articleNumber;
	DistributionStatus status;
	};
	
void displayArticles(void) {
	struct Article articleArray[] = {{{0x1101}, AVAILABLE}, {{0x1110}, BACKORDER}, {{0x0101}, SOLD_OUT}};
	
	int pos = 0;
	while (1)
	{
		waitForInput();
		waitForNoInput();
		uint8_t prodID = articleArray[pos].articleNumber.singleNumbers.productID;
		uint8_t manID = articleArray[pos].articleNumber.singleNumbers.manufactureId;
		
		lcd_init();
		lcd_line1();
		lcd_writeString("Hersteller: ");
		//Normally with uint16_t
		lcd_writeDec(manID);
		
		lcd_line2();
		lcd_writeString("Art# ");
		//Normally with uint16_t
		lcd_writeDec(prodID);
		
		switch (articleArray[pos].status)
		{
			case 0:
			lcd_writeString("Lager");
			break;
			case 1:
			lcd_writeString("RStand");
			break;
			case 99:
			lcd_writeString("AUS");
			break;
			default:
			lcd_writeString("ERR");
			break;
		}
		
		pos = (pos + 1) % 3;
	}
	
	
}