#include "TPM.h"

void initTPM2() {
	//SIM (System Integration Module
		//SCGC5 (System Clock Gating Control Register 5)
	SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC( 3 ); // 11 -> MCGIRCLK clock
	
	MCG->C1 |=
		MCG_C1_IRCLKEN_MASK | //MCGIRCLK active
		MCG_C1_IREFS_MASK; //The slow internal reference clock is selected (for FLL)
	MCG->C2 |=
		MCG_C2_IRCS_MASK; //Fast internal reference clock selected
	
	TPM2->SC &= ~TPM_SC_CPWMS_MASK; //TPM counter operates in up counting mode
	TPM2->SC |= TPM_SC_PS( 7 ); // 111 -> Divide by 128
	
	TPM2->CNT = 0x00; //clear counter
	TPM2->MOD = 15625 + 260 - 43 + (10+4+2-1-5); //CPU_INT_FAST_CLK_HZ/128 + 1/60 - 1/6 + C
	
	TPM2->SC |= TPM_SC_TOIE_MASK; //Enable TOF interrupts. An interrupt is generated when TOF equals one.
	
	NVIC_ClearPendingIRQ(TPM2_IRQn);
	NVIC_EnableIRQ(TPM2_IRQn);
	NVIC_SetPriority (TPM2_IRQn, 0); //najwyzszy priorytet
}

void TPM2_IRQHandler(void) {
	//TSI0->DATA |= TSI_DATA_SWTS_MASK;
	//TPM2->STATUS & TPM_STATUS_TOF_MASK - timer overflow
		//(nie ma kanalow, wiec to jest oczywiste)
	
	//sprawdzanie wartoci krytycznej
	if( ALARM1.state != FSM_quiet ) {
		ALARM1.n++;
		if( ALARM1.n > MAX_BUZZ_TIME ) {
			ALARM1.state = FSM_terminate;
			doAlarmFSM( &ALARM1 );
		}
	}
	if( ALARM2.state != FSM_quiet ) {
		ALARM2.n++;
		if( ALARM2.n > MAX_BUZZ_TIME ) {
			ALARM2.state = FSM_terminate;
			doAlarmFSM( &ALARM2 );
		}
	}
	
	//wlaczenie alarmu
	if( TIME.h == ALARM1.time.h && TIME.m == ALARM1.time.m && TIME.s == ALARM1.time.s ) {
		ALARM1.state = FSM_start;
		doAlarmFSM( &ALARM1 );
	}
	else if( TIME.h == ALARM2.time.h && TIME.m == ALARM2.time.m && TIME.s == ALARM2.time.s ) {
		ALARM2.state = FSM_start;
		doAlarmFSM( &ALARM2 );
	}
	
	//inkrementuj liczniki czasu
	if( ++(TIME.s) == 60 ) {
		TIME.s = 0;
		if( ++(TIME.m) == 60) {
			TIME.m = 0;
			TIME.h++;
			TIME.h %= 24;
		}
	}
	
	//zamrugaj drukropkiem
	toggleDotLCD( LCD_MASK_COLON );
	
	switch( SECONDS_FSM_STATE ) {
		
		//wyswietl nowy czas
		case FSM_display_and_sleep:
		case FSM_display:
			//setTimeLCD( TIME );
			setTimeSecondsLCD( TIME ); //debug
			break;
			
		case FSM_background:
			break;
		
		case FSM_reset_display:
			clearLCD();
			offLED( LED_MASK_ALL );
			if( ALARM1.n != 0xFFFF )
				onDotLCD( LCD_MASK_DOT1 ); //poinformuj, ze alarm1 jest aktywny
			if( ALARM2.n != 0xFFFF )
				onDotLCD( LCD_MASK_DOT3 ); //poinformuj, ze alarm2 jest aktywny
			//setTimeLCD( TIME );
			setTimeSecondsLCD( TIME ); //debug
			SECONDS_FSM_STATE = FSM_display_and_sleep;
			break;
	}
	
	TPM2->STATUS |= TPM_STATUS_TOF_MASK; //clear interrupt flag
}

void restartTPM2(volatile timeStruct newTime) {
	TPM2->SC &= ~TPM_SC_CMOD_MASK; //disables TPM2
	
	NVIC_ClearPendingIRQ(TPM2_IRQn);
	TPM2->CNT = 0x00;
	
	SECONDS_FSM_STATE = FSM_reset_display;
	
	TIME = newTime;
	
	TPM2->SC |= TPM_SC_CMOD(1); //enables TPM2
}
