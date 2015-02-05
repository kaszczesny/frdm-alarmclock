#include "MKL46Z4.h"
#include "globals.h"
#include "LED.h"
#include "LCD.h"
#include "VLPS.h"
#include "TPM.h"
#include "SW.h"
#include "BUZZ.h"
#include "TSI.h"
#include "I2C.h"
#include "ACC.h"

volatile timeStruct TIME;
volatile alarmStruct ALARM1;
volatile alarmStruct ALARM2;

volatile enum secondsFSM SECONDS_FSM_STATE = FSM_background;
volatile enum switchFSM SWITCH_FSM_STATE = FSM_quit;

volatile uint8_t TSI_VALUE = 0;
volatile uint8_t END_FLAG = 1;

//bo ten kompilator nie obsluguje inicjalizacji struktur
void initGlobals(void) {
	TIME.h = 9u;
	TIME.m = 0u;
	TIME.s = 0u;
	
	ALARM1.time.h = 9u;
	ALARM1.time.m = 0u;//15u;
	ALARM1.time.s = 10u;//0u;
	ALARM1.n = 0u;//0xFFFF;
	ALARM1.state = FSM_quiet;
	
	ALARM2.time.h = 9u;
	ALARM2.time.m = 30u;
	ALARM2.time.s = 0u;
	ALARM2.n = 0xFFFF;
	ALARM2.state = FSM_quiet;
	
}

int main(void){
	initGlobals();
	
	initLCD();
	initLED();
	initVLPS();
	initSW();
	initTPM2();
	initBUZZ();
	initI2C0();
	//initTSI();
	
	onLED( LED_MASK_ALL );
	setTimeLCD( TIME );
	onDotLCD( LCD_MASK_DOT1 | LCD_MASK_COLON );
	SWITCH_FSM_STATE = FSM_time_h;
	
	//czekamy az user ustawi czas
	while( SWITCH_FSM_STATE != FSM_quit );
	
	while(1) {
		if( SECONDS_FSM_STATE == FSM_display_and_sleep ) {
			enterVLPS();
		}
	}
}
