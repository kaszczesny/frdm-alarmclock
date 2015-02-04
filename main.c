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
volatile enum switchFSM SWITCH_FSM_STATE = FSM_time_h;

volatile uint8_t TSI_VALUE = 0;
volatile uint8_t END_FLAG = 1;

//bo ten kompilator nie obsluguje inicjalizacji struktur
void initGlobals(void) {
	TIME.h = 9u;
	TIME.m = 0u;
	TIME.s = 0u;
	
	ALARM1.time.h = 9u;
	ALARM1.time.m = 0;//15u;
	ALARM1.time.s = 5;//0u;
	ALARM1.n = 0;//0xFFFF;
	ALARM1.state = FSM_quiet;
	
	ALARM2.time.h = 9u;
	ALARM2.time.m = 30u;
	ALARM2.time.s = 0u;
	ALARM2.n = 0xFFFF;
	ALARM2.state = FSM_quiet;
	
}

int main(void){//volatile int i;int16_t z,max=0;
	initGlobals();
	
	initLCD();
	initLED();
	initVLPS();
	initSW();
	initTPM2();
	initBUZZ();
	initI2C0();
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
	
	/*enableACC();
	SECONDS_FSM_STATE = FSM_background;
	offDotLCD(LCD_MASK_DOT3);
	offLED(LED_MASK_ALL);*/
	while(1) {
		if( SECONDS_FSM_STATE == FSM_display_and_sleep ) {
			enterVLPS();
		}
//		z = getVectorACC();//readAxisValue(Z);
//		//z = z < 0 ? z*(-1) : z;
//		/*setNumberLCD( z , 10u );
//		for(i=0;i<=10000;i++);
//		toggleLED(LED_MASK_RED);
//		setNumberLCD( (uint32_t) z , 11u );
//		for(i=0;i<=10000;i++);*/
//		//toggleLED(LED_MASK_GREEN);
//		//for(i=0;i<=10000;i++);
//		for(i=0;i<=100000;i++);
//			
//		//t = getVectorACC();
//		//sum += t;
//		if( z > max )
//			max = z;
//		/*if( k % 25 == 0 ) {
//			setNumberLCD( max , 10u );
//			onLED(LED_MASK_RED);
//			for(i=0;i<=600000;i++);
//			offLED(LED_MASK_RED);
//		}
//		else {*/
//		setNumberLCD( max , 10u );
//			
//		/*}
//		k++;
//		if( k == 25*10 ) {
//			setNumberLCD( sum/k , 11u );
//			sum=0;
//			max = 0;
//			k = 0;
//			onLED(LED_MASK_GREEN);
//			for(i=0;i<=6000000;i++);
//			offLED(LED_MASK_GREEN);
//		}*/
	}
}
