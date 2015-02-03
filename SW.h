#ifndef SW_h
#define SW_h

#include "MKL46Z4.h"
#include "globals.h"
#include "LCD.h"
#include "TPM.h"
#include "TSI.h"

#define SW_PIN_SW1     3u   //PTC3
#define SW_PIN_SW3     12u  //PTC12
#define SW_PIN_SW2     20u  //PTA20

#define CLEAR_SW1 PORTC->PCR[SW_PIN_SW1] |= PORT_PCR_ISF_MASK
#define CLEAR_SW3 PORTC->PCR[SW_PIN_SW3] |= PORT_PCR_ISF_MASK
#define CLEAR_SW2 PORTA->PCR[SW_PIN_SW2] |= PORT_PCR_ISF_MASK

enum pressedButton {
	sw1,
	sw3,
	sw2,
	tsi
};

//ponizszy algorytm - zeby nie rozbijac na kilka przerwan
void handleButton( enum pressedButton button );

/** inicjalizacja odpowiednich rejestrow
 *    
 *    SW Interrupt - najpierw niezaleznie od przycisku:
 *        |
 *        v
 *    -jesli jest aktywny alarm, zignoruj
 *    -jesli quit:
 *    -zablokuj wyswietlanie czasu i VLPS
 *          |              |           |
 *          | (SW3)        | (SW1)     | (SW2)
 *          |              |           |
 *          v              v           v
 *    config alarm1  config alarm2     config czasu
 *          |              |           |
 *  ------->------>|<-------<-----------
 *  |              |                        
 *  |              v
 *  |   -SW3: dekrementuj godziny
 *  |   -SW1: inkrementuj godziny
 *  |   -specjalna wartosc przed 0, ktora dezaktywuje alarm lub anuluje ustawianie czasu
 *  --  -slider: ustaw godzine
 *      -wyswietl ustawienie
 *      -SW2: przejdz do minut -------|
 *                           |        |
 *  ------------------------>|        |
 *  |                        v        |
 *  |   -SW3: dekrementuj minuty      | (alarm i dezaktywowany)
 *  |   -SW1: inkrementuj minuty      |
 *  --  -slider: ustaw minute         |
 *      -wyswietl ustawienie          |
 *      -SW2 -------------------------+-----|
 *        |                           |     |
 *        | (alarmy)                  |     | (czas)
 *        v                           v     |
 *    -ustaw alarm                          |
 *    -odblokuj wyswietlanie czasu i VLPS   |
 *    -wyjscie z menu                       |
 *                                          v
 *         -odblokuj wyswietlanie czasu i VLPS
 *         -ustaw nowy czas
 *         -wlacz na nowo TPM2
 *         -wyjscie z menu
 *    
 *    Ta FSM implementuje menu. Wszystkie zmiany odbywaja sie na zmiennych tymczasowych
 *    (mozna brutalnie opuscic menu i konfiguracja zostanie anulowana)
 */
void initSW(void);

//wylaczenie przerwan na pinach SW1-3 (dla alarmu) oraz anulowanie menu
void disableSW(void);

//wlaczenie przerwan na pinach SW-3 (dla alarmu)
void enableSW(void);

//wewnetrzna funkcja, ktora realizuje 'zawijanie' godzin
void normalizeHour(void);

#endif
