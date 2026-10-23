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

enum DistributionStatus {
	AVAILABLE, 
	BACKORDER,
	SOLD_OUT = 99,
	};
	
	
struct ArticleNumber {
		uint8_t manufactureId; 
		uint8_t productID;
	};
	

typedef enum {
	AVAILABLE,
	BACKORDER,
	SOLD_OUT = 99,
	} DistributionStatus;
	
typedef struct {
	uint8_t productID;
	uint8_t manufactureId;
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
	Article articleArray[] = {{{0x1101}, AVAILABLE}, {{0x1110}, BACKORDER}, {{0x0101}, SOLD_OUT}};
	
	int pos = 1;
	while (1)
	{
		waitForInput();
		uint8_t prodID = articleArray[pos-1].articleNumber & 0x1100;
		uint8_t manID = articleArray[pos-1].articleNumber & 0x0011;
		
		char arr[];
		switch (articleArray[pos-1].status)
		{
		case 0:
			arr = "Lager";
			break;
		case 1:
			arr = "RStand";
			break;
		case 99:
			arr = "Aus";
			break;
		default:
			arr = "ERR";
			break;
		}
		
		lcd_init();
		lcd_line1();
		lcd_writeString("Hersteller: ");
		//Normally with uint16_t
		lcd_writeDec(manID);
		
		lcd_line2();
		lcd_writeString("Art# ");
		//Normally with uint16_t
		lcd_writeDec(prodID);
		lcd_writeString(arr);
		
		pos = (pos + 1) % 3;
	}
	
	
}