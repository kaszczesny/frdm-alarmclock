#include "LED.h"

void initLED(void) {
	//SIM (System Integration Module) - rejestr dajacy kontrole nad podawaniem zegara peryferiom
		//SCGC5 (System Clock Gating Control Register 5) - rejestr odpowiedzialny za taktowanie portow
	SIM->SCGC5 |=
		SIM_SCGC5_PORTD_MASK |
		SIM_SCGC5_PORTE_MASK; //wlaczenie takowania portow
	
	//PORT (Port Control and Interrupts) - rejestr decydujacy o tym, ktore piny wkorzystywane sa dla dancyh peryferiow
		//PCR (Pin Control Register) - sluzy do ustawienia na multiplekserze danego pinu GPIO (001)
	PORTD->PCR[LED_PIN_RED]   = PORT_PCR_MUX(1UL);
	PORTE->PCR[LED_PIN_GREEN] = PORT_PCR_MUX(1UL);
	
	//GPIO (General Purpose Input Output)
	//dla danego pinu trzeba okreslic kierunek, a potem konkretne zachowanie
		//PDDR (Direction): 1 - output, 0 - input
		//PDOR (Output)
		//PSOR (Set)
		//PCOR (Clear)
		//PTOR (Toggle)
		//PDIR (Input)
	PTD->PDDR |= LED_MASK_RED;
	PTE->PDDR |= LED_MASK_GREEN;
	
	offLED( LED_MASK_ALL );
}

/*
 * magiczna wlasnosc kodu:
 *   wedlug mnie w onLED powinno byc PSOR, a w offLED PCOR, ale wtedy nie dziala :(
 *   podobnie warunki w on/off/toggleLED powinny byc raczej "if( color & LED_MASK_COLOR )
 * no ale co ja bede dyskutowac z plytka : )
 */

void toggleLED( uint32_t color ) {
	if( !(color & LED_MASK_RED) )
		PTD->PTOR = LED_MASK_RED;
	if( !(color & LED_MASK_GREEN) )
		PTE->PTOR = LED_MASK_GREEN;
}

void onLED( uint32_t color ) {
	if( !(color & LED_MASK_RED) )
		PTD->PCOR = LED_MASK_RED;
	if( !(color & LED_MASK_GREEN) )
		PTE->PCOR = LED_MASK_GREEN;
}

void offLED( uint32_t color ) {
	if( !(color & LED_MASK_RED) )
		PTD->PSOR = LED_MASK_RED;
	if( !(color & LED_MASK_GREEN) )
		PTE->PSOR = LED_MASK_GREEN;
}

void unsetLED(void) {
	offLED( LED_MASK_ALL  );
	
	PORTD->PCR[LED_PIN_RED]   = PORT_PCR_MUX(0UL);
	PORTE->PCR[LED_PIN_GREEN] = PORT_PCR_MUX(0UL);
	
	SIM->SCGC5 &= ~(
		SIM_SCGC5_PORTD_MASK |
		SIM_SCGC5_PORTE_MASK );
}
