#ifndef TSI_h
#define TSI_h

#include "MKL46Z4.h"
#include "globals.h"
#include "SW.h"

#define TSI_PIN_CH2			1u	//PTA1
#define TSI_PIN_CH3	    2u  //PTA2

#define TOTAL_ELECTRODE 2u
#define THRESHOLD			  100u

#define TSI_FACTOR 1000

//inicjalizacja odpowiednich rejestrow
void initTSI(void);

//aktywacja pomiarow TSI
void enableTSI(void);

//dezaktywacja pomiarow TSI
void disableTSI(void);

//kalibracja
void calibrationTSI(void);

void change_electrode(void);

void readTSI(void);

#endif
