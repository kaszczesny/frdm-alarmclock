#ifndef BUZZ_h
#define BUZZ_h

#include "MKL46Z4.h"
#include "globals.h"

#define MAX_BUZZ_TIME 60*5

/** inicjalizacja odpowiednich rejestrow (TPM1 - do generowania fali 2kHz)
 *		
 *		start:
 *		-wylacz drugi alarm, jest jest wlaczony,
 *		-wyjdz z menu i ignoruj buttony
 *		-wyswietlaj godzine, ale nie spij
 *		-wlacz buzzer
 *		
 *		
 *		
 *		
 */
void initBUZZ(void);

void doAlarmFSM( volatile alarmStruct* alarm );


#endif
