#include "TSI.h"

static uint8_t ELEC_ARRAY[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
static uint16_t TSI_COUNT[16];
static uint16_t BASELINE[16];
static uint16_t DELTA[16];

void initTSI(void) {
	SIM->SCGC5 |=
		SIM_SCGC5_PORTA_MASK |
		SIM_SCGC5_TSI_MASK;  // Enable clock gating for TSI
	
	PORTA->PCR[TSI_PIN_CH2] = PORT_PCR_MUX(0u);
	PORTA->PCR[TSI_PIN_CH3] = PORT_PCR_MUX(0u);
	
	NVIC_ClearPendingIRQ(TSI0_IRQn);
	NVIC_EnableIRQ(TSI0_IRQn);
	NVIC_SetPriority (TSI0_IRQn, 3); //najnizszy priorytet
	
	//GENCS (General Control and Status)
	TSI0->GENCS |=
		TSI_GENCS_ESOR_MASK | //End-of-scan interrupt is allowed
	  TSI_GENCS_MODE(0) | //Set TSI in capacitive sensing(non-noise detection) mode
	  TSI_GENCS_REFCHRG(4) | // 100 -> 8 uA
	  TSI_GENCS_DVOLT(0) | //DV = 1.03 V; V_P = 1.33 V; V_m = 0.30 V.
	  TSI_GENCS_EXTCHRG(7) |  // 111 -> 64 uA
	  TSI_GENCS_PS(4) | // 100 -> Electrode Oscillator Frequency divided by 16
	  TSI_GENCS_NSCN(11) | // 1011 -> 12 skanow na elektrode
	  TSI_GENCS_TSIIEN_MASK; //TSI interrupt is enabled
		//TSI_GENCS_STM_MASK;     //Soft trigger scan 
	  //TSI_GENCS_STPE_MASK; //TSI is disabled when MCU goes into low power mode.
	
	enableTSI();
	
	calibrationTSI();
}

void TSI0_IRQHandler(void) {
	END_FLAG=1;
	
	 
	TSI0->GENCS |= TSI_GENCS_EOSF_MASK; // Clear End of Scan Flag
  change_electrode();
}

void enableTSI(void) {
	//TSI0->GENCS |= TSI_GENCS_TSIEN_MASK; 
}

void disableTSI(void) {
	//TSI0->GENCS &= ~TSI_GENCS_TSIEN_MASK; 
}

void calibrationTSI(void) {
	uint8_t cnt, trigger_backup;
  
  TSI0->GENCS |= TSI_GENCS_EOSF_MASK;      // Clear End of Scan Flag 
  TSI0->GENCS &= ~TSI_GENCS_TSIEN_MASK;    // Disable TSI module
  
	// Back-up TSI Trigger mode from Application 
  trigger_backup = TSI0->GENCS & TSI_GENCS_STM_MASK;
  
  TSI0->GENCS &= ~TSI_GENCS_STM_MASK;      // Use SW trigger
  TSI0->GENCS &= ~TSI_GENCS_TSIIEN_MASK;    // Enable TSI interrupts
  
  TSI0->GENCS |= TSI_GENCS_TSIEN_MASK;     // Enable TSI module
  
  for(cnt=0; cnt < TOTAL_ELECTRODE; cnt++)  // Get Counts when Electrode not pressed
  {
    TSI0->DATA = ((ELEC_ARRAY[cnt] << TSI_DATA_TSICH_SHIFT) );
    TSI0->DATA |= TSI_DATA_SWTS_MASK;
    while(!(TSI0->GENCS & TSI_GENCS_EOSF_MASK));
    TSI0->GENCS |= TSI_GENCS_EOSF_MASK;
    BASELINE[cnt] = (TSI0->DATA & TSI_DATA_TSICNT_MASK); 
  }
  
  TSI0->GENCS &= ~TSI_GENCS_TSIEN_MASK;    // Disable TSI module
  TSI0->GENCS |= TSI_GENCS_TSIIEN_MASK;     // Enale TSI interrupt
	if(trigger_backup)
		TSI0->GENCS |= TSI_GENCS_STM_MASK;            // Restore trigger mode
	else
		TSI0->GENCS &= ~TSI_GENCS_STM_MASK;
  
  TSI0->GENCS |= TSI_GENCS_TSIEN_MASK;     // Enable TSI module
  
  TSI0->DATA = ((ELEC_ARRAY[0]<<TSI_DATA_TSICH_SHIFT) );
  TSI0->DATA |= TSI_DATA_SWTS_MASK;
}

void change_electrode(void) {
	int16_t temp_delta;
	static uint8_t ongoing_elec = 0;
    
	TSI_COUNT[ongoing_elec] = (TSI0->DATA & TSI_DATA_TSICNT_MASK);          // Save Counts for current electrode
	temp_delta = TSI_COUNT[ongoing_elec] - BASELINE[ongoing_elec];  // Obtains Counts Delta from callibration reference
	if( temp_delta < 0)
		DELTA[ongoing_elec] = 0;
	else
		DELTA[ongoing_elec] = temp_delta;
	
	if(TOTAL_ELECTRODE > 1)  //Change Electrode to Scan
	{
		if((TOTAL_ELECTRODE-1) > ongoing_elec) ongoing_elec++;
		else   ongoing_elec = 0;
		
		TSI0->DATA = ((ELEC_ARRAY[ongoing_elec]<<TSI_DATA_TSICH_SHIFT) );
		TSI0->DATA |= TSI_DATA_SWTS_MASK; 
	}
}

void readTSI(void) {
	uint16_t SliderPercentegePosition[2], AbsolutePercentegePosition;
	if(!END_FLAG)
		return;
	
	END_FLAG = 0u;
	toggleLED( LED_MASK_RED );
	if((DELTA[0] > THRESHOLD)||(DELTA[1] > THRESHOLD))
	 {
		 SliderPercentegePosition[0] = (DELTA[0]*TSI_FACTOR)/(DELTA[0]+DELTA[1]);
		 
		 SliderPercentegePosition[1] = (DELTA[1]*TSI_FACTOR)/(DELTA[0]+DELTA[1]);
		 
		AbsolutePercentegePosition = ((TSI_FACTOR - SliderPercentegePosition[0]) + SliderPercentegePosition[1])/2;
		 
	 }else
	 {
			SliderPercentegePosition[0] = 0;  
			SliderPercentegePosition[1] = 0; 
			AbsolutePercentegePosition = 0; 
	 }
	
	 TSI_VALUE = AbsolutePercentegePosition;
	 //handleButton(tsi);
}
