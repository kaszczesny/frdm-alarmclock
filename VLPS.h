#ifndef VLPS_h
#define VLPS_h

#include "MKL46Z4.h"
#include "globals.h"

//inicjalizacja odpowiednich rejestrow
void initVLPS(void);

//wejscie w stan Very-Low-Power Stop (az do przerwania)
void enterVLPS(void);

#endif
