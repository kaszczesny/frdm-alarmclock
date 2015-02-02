#include "BUZZ.h"

void initBUZZ(void) {
	
}

void doAlarmFSM( volatile alarmStruct* alarm ) {
	switch( alarm->state ) {
		
		case FSM_start:
			break;
		
		case FSM_acc:
			break;
		
		case FSM_check:
			break;
		
		case FSM_terminate:
			break;
		
		case FSM_quiet:
			break;
		
	}
}
