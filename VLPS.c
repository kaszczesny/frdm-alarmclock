#include "VLPS.h"

void initVLPS(void) {
	//System Mode Controller (SMC) is responsible for sequencing the system into and out of all low-power Stop and Run modes.
	
	//PMPROT (Power Mode Protection)
	SMC->PMPROT = SMC_PMPROT_AVLP_MASK; //VLPR, VLPW and VLPS are allowed

	//w VLPS zegar MCG domyslnie nie dziala, wiec trzeba go wlaczyc
	MCG->C1 |=
		MCG_C1_IRCLKEN_MASK | //MCGIRCLK active
		MCG_C1_IREFSTEN_MASK; //Internal reference clock is enabled in Stop mode
	MCG->C2 |=
		MCG_C2_IRCS_MASK; //Fast internal reference clock selected
}

void enterVLPS(void) {
	//PMCTRL (Power Mode Control)
	SMC->PMCTRL &= ~SMC_PMCTRL_STOPM_MASK;
	SMC->PMCTRL = SMC_PMCTRL_STOPM(2); // 010 -> VLPS
	
	//SCB (System Control Block)
		//SCR (System Control Register(
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; //Set the SLEEPDEEP bit to enable deep sleep mode
	__wfi(); //assembly WFI instruction will start entry into low-power mode
}
