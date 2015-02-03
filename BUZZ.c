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
	TPM1->SC |= TPM_SC_PS( 0 ); // 0000 -> Divide by 1
	
	TPM1->CNT = 0x00; //clear counter
	TPM1->MOD = BUZZ_DEFAULT_MOD;
	
	TPM1->CONTROLS[0].CnSC = 	//output compare - toggle output on match
		TPM_CnSC_MSA_MASK |
		TPM_CnSC_ELSA_MASK |
		TPM_CnSC_CHIE_MASK; //channel interrupt
		
	TPM1->CONTROLS[0].CnV = BUZZ_DEFAULT_MOD;
	
	NVIC_ClearPendingIRQ(TPM1_IRQn);
	NVIC_EnableIRQ(TPM1_IRQn);
	NVIC_SetPriority (TPM1_IRQn, 0); //najwyzszy priorytet*/
}

void TPM1_IRQHandler(void) {
	TPM1_N++;
	if( TPM1_N % 3000 == 0 && TPM1->MOD >= 1425 ) { //proba jakiejs modulacji
		TPM1->MOD -= 25;
		TPM1->CONTROLS[0].CnSC = TPM1->MOD;
	}
	setNumberLCD( TPM1->MOD, 10u | LCD_CLEAR_LEADING_ZEROS );
	offDotLCD( LCD_MASK_DOT_ALL );
	TPM1->STATUS |= TPM_STATUS_CH0F_MASK;
}

void doAlarmFSM( volatile alarmStruct* alarm ) {
	volatile alarmStruct* other;
	
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
				//TODO: startAcc
			}
			alarm->state = FSM_clear;
			break;
		
		case FSM_clear: //wyczysc dane zebrane z akcelerometru
			alarm->N = 0;
			alarm->sum = 0;
			alarm->state = FSM_acc;
			break;
		
		case FSM_acc: //zbieraj dane z akcelerometru
			/*
			v = getVectorAcc();
			if( v...)
				alarm->state = FSM_clear;
			else {
				alarm->N++;
				alarm->sum += v
			}
			if(costam)
				alarm->state = FSM_terminate;
			*/
			//jezeli wyniki nie beda zadowalajace, alarm zostanie zabity po osiagnieciu MAX_BUZZ_TIME
			break;
		
		case FSM_terminate: //wylacz alarm
			enableSW();
			SECONDS_FSM_STATE = FSM_reset_display;
			stopTPM1();
			//TODO: stopAcc
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
