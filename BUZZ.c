#include "BUZZ.h"

static volatile uint16_t TPM1_N = 0;        //do modulacji
static volatile int8_t TPM1_direction = 1;  //do modulacji

void initBUZZ(void) {
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	PORTE->PCR[BUZZ_PIN] = PORT_PCR_MUX(3UL); //TPM1_CH0
	PTE->PDDR |= BUZZ_MASK; //output
	PTE->PCOR |= BUZZ_MASK;
	
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;

	TPM1->SC &= ~TPM_SC_CPWMS_MASK; //TPM counter operates in up counting mode
	TPM1->SC |= TPM_SC_PS( 0 ); // 000 -> Divide by 1
	
	TPM1->SC |= TPM_SC_TOIE_MASK; //overflow interrupt
	
	TPM1->CNT = 0x00; //clear counter
	TPM1->MOD = BUZZ_DEFAULT_MOD;
	
	TPM1->CONTROLS[0].CnSC = 	//output compare - toggle output on match
		TPM_CnSC_MSA_MASK |
		TPM_CnSC_ELSA_MASK;
		
	TPM1->CONTROLS[0].CnV = BUZZ_DEFAULT_MOD;
	
	NVIC_ClearPendingIRQ(TPM1_IRQn);
	NVIC_EnableIRQ(TPM1_IRQn);
	NVIC_SetPriority (TPM1_IRQn, 0); //najwyzszy priorytet*/
	
	
	
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

	TPM0->SC &= ~TPM_SC_CPWMS_MASK; //TPM counter operates in up counting mode
	TPM0->SC |= TPM_SC_PS( 4 ); // 100 -> Divide by 16
	
	TPM0->SC |= TPM_SC_TOIE_MASK; //overflow interrupt
	
	TPM0->CNT = 0x00; //clear counter
	TPM0->MOD = TPM2_ONE_SECOND * (128/16) / BUZZ_STEP_FREQ;
	
	
	NVIC_ClearPendingIRQ(TPM0_IRQn);
	NVIC_EnableIRQ(TPM0_IRQn);
	NVIC_SetPriority (TPM0_IRQn, 1); //wysoki priorytet*/
	
}

void TPM0_IRQHandler(void) {
	doAlarmFSM( &ALARM1 );
	doAlarmFSM( &ALARM2 );
	TPM0->STATUS |= TPM_STATUS_TOF_MASK; //clear interrupt flag
}

void TPM1_IRQHandler(void) {
	uint16_t mod = TPM1->MOD;
	TPM1_N++;
	if( TPM1_N % 40 == 0 && mod > 500 ) { //proba jakiejs modulacji
		//mod = TPM1->MOD;
		//TPM1->SC &= ~TPM_SC_CMOD_MASK;
		//TPM1->CNT = 0;
		TPM1->MOD = mod - 1;
		TPM1->CONTROLS[0].CnV = mod - 1;
		//TPM1->SC |= TPM_SC_CMOD(1);
	}
	setNumberLCD( mod, 11u );
	offDotLCD( LCD_MASK_DOT_ALL );
	TPM1->STATUS |= TPM_STATUS_TOF_MASK; //clear interrupt flag
}

void doAlarmFSM( volatile alarmStruct* alarm ) {
	volatile alarmStruct* other;
	uint32_t v;
	
	switch( alarm->state ) {
		
		case FSM_quiet: //alarm jest nieaktywny
			break;
		
		case FSM_start: //wlacz alarm
			if( alarm == &ALARM1 )
				other = &ALARM2;
			else
				other = &ALARM1;
			
			if( other->state != FSM_quiet ) { //wylaczenie drugiego alarmu
				other->state = FSM_quiet;
				other->n = 0;
				other->N = 0;
				other->sum = 0;
				//jesli drugi alarm byl wlaczony, to buttony, menu, ACC itp. sa w dobrym stanie
			}
			else {
				SWITCH_FSM_STATE = FSM_quit;
				disableSW();
				SECONDS_FSM_STATE = FSM_background;//FSM_display; - debug do modulacji
				offLED( LED_MASK_ALL );
				startTPM1();
				enableACC();
				startTPM0();
			}
			alarm->state = FSM_clear;
			break;
		
		case FSM_clear: //wyczysc dane zebrane z akcelerometru
			alarm->N = 0;
			alarm->sum = 0;
			alarm->state = FSM_acc;
			break;
		
		case FSM_acc: //zbieraj dane z akcelerometru
			v = getVectorACC();
			if( v < BUZZ_THRESHOLD_ACC ) {
				alarm->state = FSM_clear;
				if( v > 650 ) //nie ma sensu swiecic, jezeli plytka lezy
					onLED( LED_MASK_RED );
				return;
			}
			
			alarm->N++;
			alarm->sum += v;
			
			if( alarm->N == BUZZ_HIGHLIGHT_TICKS )
				offLED( LED_MASK_ALL );
			
			if( alarm->N >= BUZZ_REQUIRED_TICS ) {
				if( (alarm->sum / alarm->N) >= BUZZ_MIN_AVG )
					alarm->state = FSM_terminate;
				else {
					alarm->state = FSM_clear;
					onLED( LED_MASK_GREEN );
				}
			}
			//jezeli wyniki nie beda zadowalajace, alarm zostanie zabity po osiagnieciu BUZZ_MAX_TIME
			break;
		
		case FSM_terminate: //wylacz alarm
			enableSW();
			SECONDS_FSM_STATE = FSM_reset_display;
			stopTPM1();
			stopTPM0();
			disableACC();
			alarm->n = 0;
			alarm->N = 0;
			alarm->sum = 0;
			alarm->state = FSM_quiet;
			break;
	}
}

void startTPM1(void) {
	TPM1->CNT = 0x00;
	TPM1->MOD = BUZZ_DEFAULT_MOD;
	
	TPM1_N = 0;
	TPM1_direction = 1;
	
	TPM1->SC |= TPM_SC_CMOD(1);
}

void stopTPM1(void) {
	TPM1->SC &= ~TPM_SC_CMOD_MASK;
	NVIC_ClearPendingIRQ(TPM1_IRQn);
	PTE->PCOR |= BUZZ_MASK;
}

void startTPM0() {
	TPM0->CNT = 0x00;
	TPM0->SC |= TPM_SC_CMOD(1);
}

void stopTPM0() {
	TPM0->SC &= ~TPM_SC_CMOD_MASK;
	NVIC_ClearPendingIRQ(TPM0_IRQn);
}
