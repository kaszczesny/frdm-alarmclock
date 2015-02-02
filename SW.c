#include "SW.h"

static volatile timeStruct tempTime; //zmienna robocza do ustawiania czasu i alarmu
static volatile uint16_t tempN = 0u; //zmienna robocza do ewentualnego wylaczania alarmu
static volatile alarmStruct* whichAlarm = 0; //wskaznik na modyfikowany alarm

void initSW(void) {
	tempTime.h = 9u;
	tempTime.m = 0u;
	tempTime.s = 0u;
	
	SIM->SCGC5 |=
		SIM_SCGC5_PORTA_MASK | //Enable clock for port A
		SIM_SCGC5_PORTC_MASK; //Enable clock for port C
	
	// 001 - GPIO
	PORTC->PCR[SW_PIN_SW1] |= PORT_PCR_MUX(1);
	PORTC->PCR[SW_PIN_SW3] |= PORT_PCR_MUX(1);
	
	// 111 - RESET_b
	PORTA->PCR[SW_PIN_SW2] |= PORT_PCR_MUX(7);

	PORTC->PCR[SW_PIN_SW1] |= 
		PORT_PCR_PE_MASK | //Internal pullup or pulldown resistor is enabled
		PORT_PCR_PS_MASK;  //Internal pullup resistor is enabled
	PORTC->PCR[SW_PIN_SW3] |= 
		PORT_PCR_PE_MASK | //Internal pullup or pulldown resistor is enabled
		PORT_PCR_PS_MASK;  //Internal pullup resistor is enabled
	PORTA->PCR[SW_PIN_SW2] |= 
		PORT_PCR_PE_MASK | //Internal pullup or pulldown resistor is enabled
		PORT_PCR_PS_MASK;  //Internal pullup resistor is enabled
	
	enableSW();
	
	NVIC_ClearPendingIRQ(PORTC_PORTD_IRQn);
	NVIC_EnableIRQ(PORTC_PORTD_IRQn);
	NVIC_SetPriority (PORTC_PORTD_IRQn, 2); //niski priorytet
	
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_SetPriority (PORTA_IRQn, 2); //niski priorytet
}

void PORTC_PORTD_IRQHandler(void) {
	enum pressedButton button;
	
	//determine what was pressed
	if( PORTC->PCR[SW_PIN_SW1] & PORT_PCR_ISF_MASK )
		button = sw1;
	else
		button = sw3;

	handleButton(button);
	
	if( button == sw1 )
		CLEAR_SW1;
	else
		CLEAR_SW3;
}

void PORTA_IRQHandler(void) {
	handleButton( sw2 );
	CLEAR_SW2;
}

void handleButton( enum pressedButton button ) {
	if( (ALARM1.state == FSM_quiet ) &&
			(ALARM2.state == FSM_quiet )
	) {
	
		switch( SWITCH_FSM_STATE ) {
			
			case FSM_quit: //user wszedl do menu po raz pierwszy
				SECONDS_FSM_STATE = FSM_background;
				switch( button ) {
					
					case sw3:
						whichAlarm = &ALARM1;
						tempTime = whichAlarm->time;
						tempN = whichAlarm->n;
						onLED( LED_MASK_RED );
						SWITCH_FSM_STATE = FSM_alarm_h;
						break;
					
					case sw1:
						whichAlarm = &ALARM2;
						tempTime = whichAlarm->time;
						tempN = whichAlarm->n;
						onLED( LED_MASK_GREEN );
						SWITCH_FSM_STATE = FSM_alarm_h;
						break;
					
					case sw2:
						tempTime = TIME;
						tempN = 0;
						onLED( LED_MASK_ALL );
						SWITCH_FSM_STATE = FSM_time_h;
						break;
				}
				tempTime.s = 0;
				clearLCD();
				if( tempN == 0xFFFF )
					setNumberLCD( LCD_DIABLED_MSG, 16u );
				else
					setTimeLCD( tempTime );
				onDotLCD( LCD_MASK_DOT1 );
				enableTSI();
				break;
				
			case FSM_alarm_h:
				switch( button ) {
					
					case sw1:
						tempTime.h++;
						if( tempTime.h == 24 ) {
							tempN = 0xFFFF;
							setNumberLCD( LCD_DIABLED_MSG, 16u );
						}
						else if( tempTime.h > 24 ) {
							tempN = 0;
							tempTime.h = 0;
							setTimeLCD( tempTime );
						}
						else
							setTimeLCD( tempTime );
						break;
					
					case sw3:
						tempTime.h--;
						if( tempTime.h == 0xFF )
								tempTime.h = 24; //normalizuj
						if( tempTime.h == 24 ) {
							tempN = 0xFFFF;
							setNumberLCD( LCD_DIABLED_MSG, 16u );
						}
						else if( tempTime.h > 24 ) {
							tempN = 0;
							tempTime.h = 0;
							setTimeLCD( tempTime );
						}
						else
							setTimeLCD( tempTime );
						break;
					
					case tsi:
						tempTime.h = (uint8_t)((TSI_VALUE * 23 ) / TSI_FACTOR );
						setTimeLCD( tempTime );
						break;
					
					//przejdz dalej
					case sw2:
						if( tempN == 0xFFFF ) { //wylacz alarm
							whichAlarm->n = tempN; //ustaw alarm
							SECONDS_FSM_STATE = FSM_reset_display;
							disableTSI();
							SWITCH_FSM_STATE = FSM_quit;
						}
						else { //przejdz do minut
							toggleDotLCD( LCD_MASK_DOT1 | LCD_MASK_DOT3 );
							tempN = 0;
							SWITCH_FSM_STATE = FSM_alarm_m;
						}
						break;
				}
				break;
				
			case FSM_time_h:
				switch( button ) {
					
					case sw1:
						tempTime.h++;
						tempTime.h %= 24;
						setTimeLCD( tempTime );
						break;
					
					case sw3:
						tempTime.h--;
						if( tempTime.h == 0xFF )
								tempTime.h = 23;
						setTimeLCD( tempTime );
						break;
					
					case tsi:
						tempTime.h = (uint8_t)((TSI_VALUE * 23 ) / TSI_FACTOR );
						setTimeLCD( tempTime );
						break;
						
					//przejdz dalej
					case sw2:
						toggleDotLCD( LCD_MASK_DOT1 | LCD_MASK_DOT3 );
						SWITCH_FSM_STATE = FSM_time_m;
						break;
				}
				break;
				
			case FSM_alarm_m:
				switch( button ) {
					
					case sw1:
						tempTime.m++;
						tempTime.m %= 60;
						setTimeLCD( tempTime );
						break;
					
					case sw3:
						tempTime.m--;
						if( tempTime.m == 0xFF )
								tempTime.m = 0;
						setTimeLCD( tempTime );
						break;
					
					case tsi:
						tempTime.m = (uint8_t)((TSI_VALUE * 59 ) / TSI_FACTOR );
						setTimeLCD( tempTime );
						break;
						
					//przejdz dalej
					case sw2:
						whichAlarm->time = tempTime; //ustaw alarm
						whichAlarm->n = 0;
						SECONDS_FSM_STATE = FSM_reset_display;
						disableTSI();
						SWITCH_FSM_STATE = FSM_quit;
						break;
				}
				break;
				
			case FSM_time_m:
				switch( button ) {
					
					case sw1:
						tempTime.m++;
						tempTime.m %= 60;
						setTimeLCD( tempTime );
						break;
					
					case sw3:
						tempTime.m--;
						if( tempTime.m == 0xFF )
								tempTime.m = 59;
						setTimeLCD( tempTime );
						break;
					
					case tsi:
						tempTime.m = (uint8_t)((TSI_VALUE * 59 ) / TSI_FACTOR );
						setTimeLCD( tempTime );
						break;
						
					//przejdz dalej
					case sw2:
						SWITCH_FSM_STATE = FSM_quit;
						restartTPM2( tempTime );
						disableTSI();
						break;
				}
				break;
		}
	}
}

void disableSW(void) {
	// 0000 -> interrupt request disabled
	PORTC->PCR[SW_PIN_SW1] |= PORT_PCR_IRQC(0u);
	PORTC->PCR[SW_PIN_SW3] |=	PORT_PCR_IRQC(0u);
	PORTA->PCR[SW_PIN_SW2] |=	PORT_PCR_IRQC(0u);
}

void enableSW(void) {
	// 1001 -> interrupt on rising edge (zwolnienie przycisku)
	PORTC->PCR[SW_PIN_SW1] |= PORT_PCR_IRQC(9u);
	PORTC->PCR[SW_PIN_SW3] |=	PORT_PCR_IRQC(9u);
	PORTA->PCR[SW_PIN_SW2] |=	PORT_PCR_IRQC(9u);
}
