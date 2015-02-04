#ifndef BUZZ_h
#define BUZZ_h

#include "MKL46Z4.h"
#include "globals.h"
#include "SW.h"
#include "ACC.h"
#include "TPM.h"

#define BUZZ_PIN              20u //PTE20
#define BUZZ_MASK             ( 1u << BUZZ_PIN )

#define BUZZ_DEFAULT_MOD      2500//1739; //CPU_INT_FAST_CLK_HZ/2300
#define BUZZ_MAX_TIME         75u //maksymalny czas piszczenia
#define BUZZ_STEP_FREQ        1000u //ile razy na sekunde jest wywolywana FSM
#define BUZZ_HIGHLIGHT_TIME   10 //przez ile sekund maja sie palic diody po zlym machaniu
#define BUZZ_THRESHOLD_ACC		2048//3328 //3.25g - nie wolno ani przez chwile machac wolniej niz tak
#define BUZZ_MIN_AVG          3072//3840 //3.75g - srednia machania nie moze byc mniejsza niz to
#define BUZZ_REQUIRED_TIME		3 //przez ile sekund trzeba machac

#define BUZZ_HIGHLIGHT_TICKS  (BUZZ_STEP_FREQ * BUZZ_HIGHLIGHT_TIME)
#define BUZZ_REQUIRED_TICS    (BUZZ_STEP_FREQ * BUZZ_REQUIRED_TIME)

/** inicjalizacja odpowiednich rejestrow (TPM1 - do generowania fali 2kHz)
 *    
 *      start:
 *      -wylacz drugi alarm, jest jest wlaczony, \
 *      -wyjdz z menu i ignoruj buttony           \
 *      -wyswietlaj godzine, ale nie spij          \
 *      -wlacz TPM0, TPM1 i ACC                     \
 *  (RED)    |                                       \
 *  -------->|<----------------                       \
 *  |        v                |                        \
 *  |  clear:                 |                         >---|
 *  |  -wyczysc dane z ACC    |                        /    |
 *  |     |                   |                       /     |
 *  |     |                   | (GREEN)              /      |
 *  |     v                   |                     /       |
 *  |  acc:                   | (zla suma)         /        |
 *  |   -zbieraj dane         ^                   /         |
 *  |   -sprawdz sume --------|                  /          |
 *  |             |           v                             |
 *  |--------------           |                             |
 *   (za duza odchylka        |                             |
 *   pojedynczego punktu)     | (dobra suma)                |
 *                            |                             |
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
