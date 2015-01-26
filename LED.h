#ifndef LED_h
#define LED_h

#include "MKL46Z4.h"

//zgodnie z User Manual Red jest na PTD5, a Green na PTE29
#define LED_PIN_RED			5u
#define LED_PIN_GREEN		29u
#define LED_MASK_RED 		(1UL << LED_PIN_RED)
#define LED_MASK_GREEN 	(1UL << LED_PIN_GREEN)
#define LED_MASK_ALL 		(LED_MASK_RED | LED_MASK_GREEN)

//inicjalizacja odpowiednich rejestrow
void initLED(void);

//zmienia stan diody na przeciwny (moze przyjac sume masek)
void toggleLED( uint32_t color );

//zapala diode (moze przyjac sume masek)
void onLED( uint32_t color );

//gasi diode (moze przyjac sume masek)
void offLED( uint32_t color );

#endif
