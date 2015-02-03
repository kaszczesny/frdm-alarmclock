#ifndef GLOBALS_h
#define GLOBALS_h

#include "MKL46Z4.h"

typedef struct timeStruct {
	uint8_t s;
	uint8_t m;
	uint8_t h;
} timeStruct;

enum secondsFSM {
	FSM_display_and_sleep,     //wyswietlenie nowej godziny + VLPS
	FSM_display,               //tylko wyswietlenie nowej godziny (podczas alarmu)
	FSM_background,            //inkrementuj w tle, nie spij
	FSM_reset_display          //wyczysc wyswietlacz i LEDy ze smieci
};

enum switchFSM {
	FSM_quit,                  //menu jest wylaczone
	FSM_alarm_h,               //ustawianie godziny dla alarmu
	FSM_time_h,                //ustawianie godziny dla nowego czasu
	FSM_alarm_m,               //ustawianie minuty dla alarmu
	FSM_time_m                 //ustawianie minuty dla nowego czasu
};

enum alarmFSM {
	FSM_start,                 //aktywuj alarm
	FSM_clear,                 //resetuj zebrane dane z akcelerometru
	FSM_acc,                   //zbieraj dane z akcelerometru
	FSM_terminate,             //'destruktor'
	FSM_quiet                  //alarm nieaktywny
};

typedef struct alarmStruct {
	volatile timeStruct time;  //czas alarmu
	uint16_t n;                //od ilu sekund dzwoni lub czy jest w ogole wylaczony (0xFFFF)
	int32_t sum;               //dane z akcelerometru
	int16_t N;                 //liczba danych z akcelerometru (do liczenia sredniej)
	enum alarmFSM state;       //stan FSM - kazy alarm ma osobna
} alarmStruct;

extern volatile timeStruct TIME;    //aktualny czas: inkrementowany w TPM.c, konfigurowany w SW.c
extern volatile alarmStruct ALARM1; //alarm1
extern volatile alarmStruct ALARM2; //alarm2

extern volatile enum secondsFSM SECONDS_FSM_STATE;  //stan glownego 'procesu'
extern volatile enum switchFSM SWITCH_FSM_STATE;    //stan menu

extern volatile uint8_t TSI_VALUE;  //smieci z TSI
extern volatile uint8_t END_FLAG;   //smieci z TSI

#endif
