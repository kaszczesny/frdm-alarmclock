#ifndef BUZZ_h
#define BUZZ_h

#include "MKL46Z4.h"
#include "globals.h"
#include "SW.h"

#define BUZZ_PIN          20u //PTE20
#define BUZZ_MASK         ( 1u << BUZZ_PIN )

#define BUZZ_DEFAULT_MOD  2000//1739; //CPU_INT_FAST_CLK_HZ/2300
#define MAX_BUZZ_TIME     7u //60*5

/** inicjalizacja odpowiednich rejestrow (TPM1 - do generowania fali 2kHz)
 *    
 *      start:
 *      -wylacz drugi alarm, jest jest wlaczony, \
 *      -wyjdz z menu i ignoruj buttony           \
 *      -wyswietlaj godzine, ale nie spij          \
 *      -wlacz TPM1 i ACC                           \
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
 *                     -wylacz TPM1 i ACC
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

#endif
