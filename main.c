#include "MKL46Z4.h"
#include "globals.h"
#include "LED.h"
#include "LCD.h"
#include "VLPS.h"
#include "TPM.h"
#include "SW.h"
#include "BUZZ.h"
#include "TSI.h"

volatile timeStruct TIME;
volatile alarmStruct ALARM1;
volatile alarmStruct ALARM2;

volatile enum secondsFSM SECONDS_FSM_STATE = FSM_background;
volatile enum switchFSM SWITCH_FSM_STATE = FSM_time_h;

volatile uint8_t TSI_VALUE = 0;
volatile uint8_t END_FLAG = 1;

void initGlobals(void) {
	TIME.h = 9u;
	TIME.m = 0u;
	TIME.s = 0u;
	
	ALARM1.time.h = 9u;
	ALARM1.time.m = 15u;
	ALARM1.time.s = 0u;
	ALARM1.n = 0xFFFF;
	ALARM1.state = FSM_quiet;
	
	ALARM2.time.h = 9u;
	ALARM2.time.m = 30u;
	ALARM2.time.s = 0u;
	ALARM2.n = 0xFFFF;
	ALARM2.state = FSM_quiet;
	
}

int main(void){volatile uint32_t i;
	initGlobals();
	
	initLCD();
	initLED();
	initVLPS();
	initSW();
	initTPM2();
	initBUZZ();
	//initTSI();
	
	/*SWITCH_FSM_STATE = FSM_quit;
	enableTSI();
	restartTPM2(TIME);
	
	while(1) {
		readTSI();
		//for(i=0;i<=250000;i++);
		setNumberLCD( TSI_VALUE, 11u );
		readTSI();
		//TSI0->DATA |= TSI_DATA_SWTS_MASK;
		//toggleLED( LED_MASK_GREEN );
	}*/
	
	
	onLED( LED_MASK_ALL );
	setTimeLCD( TIME );
	onDotLCD( LCD_MASK_DOT1 | LCD_MASK_COLON );
	
	//czekamy az user ustawi czas
	while( SWITCH_FSM_STATE != FSM_quit );
	
	
	while(1) {
		if( SECONDS_FSM_STATE == FSM_display_and_sleep ) {
			enterVLPS();
		}
	}
}
