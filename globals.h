#ifndef GLOBALS_h
#define GLOBALS_h

#include "MKL46Z4.h"

typedef struct timeStruct {
	uint8_t s;
	uint8_t m;
	uint8_t h;
} timeStruct;

enum secondsFSM {
	FSM_display_and_sleep, //wyswietlenie nowej godziny + VLPS
	FSM_display, //tylko wyswietlenie nowej godziny (podczas alarmu)
	FSM_background, //inkrementuj w tle, nie spij
	FSM_reset_display
};

enum switchFSM {
	FSM_quit,
	
	FSM_alarm_h,
	FSM_time_h,
	
	FSM_alarm_m,
	FSM_time_m
};

enum alarmFSM {
	FSM_start,
	FSM_acc,
	FSM_check,
	FSM_terminate,
	FSM_quiet
};

typedef struct alarmStruct {
	volatile timeStruct time;
	uint16_t n; //od ilu sekund dzwoni lub stan (0xFFFF - wylaczony)
	int32_t sum; //dane z akcelerometru
	int16_t N; //liczba danych z akcelerometru
	enum alarmFSM state;
} alarmStruct;

extern volatile timeStruct TIME; //aktualny czas: inkrementowany w TPM.c, ustawiany w SW.c
extern volatile alarmStruct ALARM1;
extern volatile alarmStruct ALARM2;

extern volatile enum secondsFSM SECONDS_FSM_STATE;
extern volatile enum switchFSM SWITCH_FSM_STATE;

extern volatile uint8_t TSI_VALUE;
extern volatile uint8_t END_FLAG;

#endif
