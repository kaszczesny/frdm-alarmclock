#ifndef TPM_h
#define TPM_h

#include "MKL46Z4.h"
#include "globals.h"
#include "LED.h"
#include "LCD.h"
#include "BUZZ.h"

/** inicjalizacja odpowiednich rejestrow
 *    TPM2 przepelnia sie co 1 sek, ew. budzac procesor i ew. wyswietlajac nowy czas w przerwaniu
 *
 *    TPM2 Overflow Interrupt  <--------------------- 1 sek <-------------------------------------
 *        |                                                                                      |
 *        v                                                                                      |
 *    -sprawdz, czy alarm przekroczyl w. krytyczna (zakonczenie 'procesu')                       |
 *    -sprawdz, czy zaczal sie alarm (uruchomienie 'procesu')                                    |
 *    -inkrementuj czas i zamrugaj dwukropkiem                                                   |
 *    -jezeli !background (uzytkownik NIE jest w menu), wyswietl nowy czas                       |
 *        |           |                                                                          |
 *        v           ---------------------------------------------------------------------------|
 *    VLPS (main)                            !display_and_sleep                                  ^
 *        |            (jezeli user jest w menu lub dzwoni alarm, to nie wchodz do VLPS w main)  |
 *        |                                                                                      |
 *        ----------------------------------------------------------------------------------------
 *
 */
void initTPM2(void);

//ustawia nowy czas i restartuje TPM2
void restartTPM2(volatile timeStruct newTime);

#endif
