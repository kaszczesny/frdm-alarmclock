#ifndef BUZZ_h
#define BUZZ_h

#include "MKL46Z4.h"
#include "globals.h"
#include "SW.h"
#include "ACC.h"
#include "TPM.h"

#define BUZZ_PIN              20u //PTE20
#define BUZZ_MASK             ( 1u << BUZZ_PIN )

//ustawienia samego piszczenia
#define BUZZ_MAX_MOD          2500u   //minimalna czestotliwosc
#define BUZZ_MIN_MOD          175u    //maksymalna czestotliwosc
#define BUZZ_MOD_STEP					5       //o ile zmieniac czestotliwosc
#define BUZZ_MOD_DELAY        700     //co ile zmieniac czestotliwosc
#define BUZZ_MAX_TIME         600u    //maksymalny czas piszczenia

//ustawienia alarmu
#define BUZZ_STEP_FREQ        50u     //ile razy na sekunde jest wywolywana FSM (50, bo tak probkuje akcelerometr)
#define BUZZ_HIGHLIGHT_TIME   1       //przez ile sekund ma sie palic zielona dioda po za malej sredniej
#define BUZZ_MAX_FAILRUES     15      //ile razy pod rzad przyspieszenie moze nie przekroczyc BUZZ_THRESHOLD_ACC
#define BUZZ_THRESHOLD_ACC		1536    //1.5g - przez dluzej niz BUZZ_MAX_FAILURES nie wolno machac wolniej niz tak
#define BUZZ_MIN_AVG          3072    //3g - srednia machania nie moze byc mniejsza niz to (srednia uwzglednia zerowe 'probki' przy konsekutywnych failurach
#define BUZZ_REQUIRED_TIME		8       //przez ile sekund trzeba machac

#define BUZZ_HIGHLIGHT_TICKS  (BUZZ_STEP_FREQ * BUZZ_HIGHLIGHT_TIME)
#define BUZZ_REQUIRED_TICS    (BUZZ_STEP_FREQ * BUZZ_REQUIRED_TIME)

/** inicjalizacja odpowiednich rejestrow (TPM1 - do generowania fali 2kHz)
 *    
 *      start:
 *      -wylacz drugi alarm, jest jest wlaczony, \
 *      -wyjdz z menu i ignoruj buttony           \
 *      -wyswietlaj godzine, ale nie spij          \
 *      -wlacz TPM0, TPM1 i ACC                     \
 *  *        |                                       \
 *  -------->|<----------------                       \
 *  |        v                |                        \
 *  |  clear:                 |                         >---|
 *  |   -wyczysc dane z ACC   |                        /    |
 *  |     |                   |                       /     |
 *  |     |<--------          | (GREEN na ~1s)       /      |
 *  |     v        | (RED)    |                     /       |
 *  |  acc:        |          | (zla suma)         /        |
 *  |-  -zbieraj dane         ^                   /         |
 *      -sprawdz sume --------|                  /          |
 *                            v                             |
 *                            |                             |
 *   (* za duzo odchylek      |                             |
 *   pojedynczego punktu      | (dobra suma)                |
 *        z rzedu)            |                             |
 *                            |<-----------------------------
 *                            v
 *                     terminate:
 *                     -wlacz buttony     
 *                     -wyswietlaj godzine i spij
 *                     -wylacz TPM0, TPM1 i ACC
 *                     -wyzeruj dane zwiazane z alarmem
 *                         |
 *                         v
 *                       quiet
 *    
 */
void initBUZZ(void);

//sprawia, ze FSM alarmu wykonuje nastepny krok
	//TODO: powiazac z jakims timerem albo z przerwaniem z akcelerometru
void doAlarmFSM( volatile alarmStruct* alarm );

//wlacza TPM1 (czyli de facto wlacza buzzer)
void startTPM1(void);

//wylacza TPM1
void stopTPM1(void);

//wlacza TPM0 (ktory sprawia, ze FSM zbiera dane z akcelerometru i pracuje)
void startTPM0(void);

//wylacza TPM0
void stopTPM0(void);

#endif
