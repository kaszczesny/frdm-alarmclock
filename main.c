#include "MKL46Z4.h"
#include "LED.h"
#include "LCD.h"
#include "RTC.h"

int main(void){
	
	//LCD + LED tester
	/*
	uint32_t i;
	timeStruct time;
	time.s = 50u;
	time.m = 58u;
	time.h = 23u;
	
	initLCD();
	initLED();
	onDotLCD( LCD_MASK_DOT_ALL );
	for( i=0; i<=93750*8*2; i++);
	offDotLCD( LCD_MASK_DOT3 );
	toggleDotLCD( LCD_MASK_DOT2 | LCD_MASK_COLON );
	for( i=0; i<=93750*8*2; i++);
	offDotLCD( LCD_MASK_DOT_ALL );
	onDotLCD( LCD_MASK_DOT1 | LCD_MASK_DOT3 );
	while(1) {
		setTimeLCD( &time );
		for( i=0; i<=93750*8; i++); //simulate roughly 1 second
		if( time.s % 20 == 0 ) {
			toggleDotLCD( LCD_MASK_COLON );
			toggleLED( LED_MASK_RED );
		}
		if( time.h % 2 == 0)
			onLED( LED_MASK_GREEN );
		else
			offLED( LED_MASK_GREEN );
	}
	*/
	while(1);
}
