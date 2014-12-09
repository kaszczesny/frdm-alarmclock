#include "LCD.h"

//static global variables are visible only in this file
const static uint8_t LCD_Front_Pin[LCD_NUM_FRONT_PINS] = {
	LCD_FRONT0_PIN,
	LCD_FRONT1_PIN,
	LCD_FRONT2_PIN,
	LCD_FRONT3_PIN,
	LCD_FRONT4_PIN,
	LCD_FRONT5_PIN,
	LCD_FRONT6_PIN, 
	LCD_FRONT7_PIN };
/*const static uint8_t LCD_Back_Pin[LCD_NUM_BACKP_PINS] = {
	LCD_BACKP0_PIN,
	LCD_BACKP1_PIN,
	LCD_BACKP2_PIN,
	LCD_BACKP3_PIN };*/


void initLCD(void) {
	//SIM (System Integration Module) - rejestr dajacy kontrole nad podawaniem zegara peryferiom
		//SCGC5 (System Clock Gating Control Register 5) - rejestr odpowiedzialny za taktowanie portow
		//zgodnie z User Manual (s. 13) i Reference Manual (s. ~175) piny LCD znajduja sie w ponizszych portach (wszystkie poza A)
	//Reference Manual s. 913 zawiera pelniejsza sekwencje
	SIM->SCGC5 |=
		SIM_SCGC5_PORTB_MASK | 
		SIM_SCGC5_PORTC_MASK | 
		SIM_SCGC5_PORTD_MASK | 
		SIM_SCGC5_PORTE_MASK | 
		SIM_SCGC5_SLCD_MASK;  //wlaczenie takowania portow

	//"If desired, set PAD_SAFE bit to prevent unwanted operation on LCD pins during configuration change."
		//GCR (General Control Register) - kontroluje wiekszosc opcji
	LCD->GCR |= LCD_GCR_PADSAFE_MASK;
	
	//wylaczenie LCD na czas konfiguracji
	LCD->GCR &= ~LCD_GCR_LCDEN_MASK;
	
	//PORT (Port Control and Interrupts) - rejestr decydujacy o tym, ktore piny wkorzystywane sa dla dancyh peryferiow
		//PCR (Pin Control Register) - dla normalnej pracy LCD trzeba ustawic 000 na multiplekserze
	PORTB->PCR[7]  = PORT_PCR_MUX(0UL);     //set PTB7  to LCD_P7
	PORTB->PCR[8]  = PORT_PCR_MUX(0UL);     //set PTB8  to LCD_P8
	PORTB->PCR[10] = PORT_PCR_MUX(0UL);     //set PTB10 to LCD_P10
	PORTB->PCR[11] = PORT_PCR_MUX(0UL);     //set PTB11 to LCD_P11
	PORTB->PCR[21] = PORT_PCR_MUX(0UL);     //set PTB21 to LCD_P17
	PORTB->PCR[22] = PORT_PCR_MUX(0UL);     //set PTB22 to LCD_P18
	PORTB->PCR[23] = PORT_PCR_MUX(0UL);     //set PTB23 to LCD_P19
	PORTC->PCR[17] = PORT_PCR_MUX(0UL);     //set PTC17 to LCD_P37
	PORTC->PCR[18] = PORT_PCR_MUX(0UL);     //set PTC18 to LCD_P38
	PORTD->PCR[0]  = PORT_PCR_MUX(0UL);     //set PTD0  to LCD_P40
	PORTE->PCR[4]  = PORT_PCR_MUX(0UL);     //set PTE4  to LCD_P52
	PORTE->PCR[5]  = PORT_PCR_MUX(0UL);     //set PTE5  to LCD_P53


	LCD->GCR = //s. 871
		//clear RVEN (disable voltage regulator)
		LCD_GCR_RVTRIM(0x00) |	//Regulated Voltage Trim
		LCD_GCR_CPSEL_MASK |		//Charge Pump Select (capacitor charge pump)
		LCD_GCR_LADJ(0x03) |		//Load Adjust; (0x03 & CPSEL = 1) ==> slow clock rate = lower power, but higher load capacitance on the LCD requires higher clock speed
		LCD_GCR_VSUPPLY_MASK |	//Voltage Supply Control (drive V_LL3 externally)
		LCD_GCR_PADSAFE_MASK |
		//clear FDCIEN (no interrupt from fault detection)
		LCD_GCR_ALTDIV(0x00) |	//Alternate Clock Divider; 00->1, 01->8,  10->64, 11->512
		//clear ALTSOURCE ( 0 -> Alternate Clock MCGIRCLK)
		//clear FFR (standard Frame Rate)
		//clear LCDDOZE (run even in doze mode)
		//clear LCDSTP (run even in stop mode)
		//clear LCDEN (disable LCD, as configuration isn't done yet)
		LCD_GCR_SOURCE_MASK |		//Clock Source Select (get from ALTSOURCE)
		LCD_GCR_LCLK(0x01) |		//LCD Clock Prescaler
		LCD_GCR_DUTY(0x03);			//LCD Duty Select (011->4BP, because there are 3 backplane pins)

	//AR (Auxiliary Register) - responsible for blinking
	LCD->AR =
		//clear BLINK (disable blinking)
		//clear ALT
		//clear BLANK
		//clear BMODE
		LCD_AR_BRATE(0x03);
	
	//FDCR (Fault Detect Control Register)
	LCD->FDCR = 0x00000000; //clear all bits in FDCR
	
	//PEN (Pin Enable)
		//for pins >= 32, set in LCD->PEN[1] as LCD_PEN_PEN(Pin#-32)
	LCD->PEN[0] =
		LCD_PEN_PEN(1u << LCD_FRONT2_PIN) |   				 //LCD_P7
		LCD_PEN_PEN(1u << LCD_FRONT3_PIN) | 				   //LCD_P8
		LCD_PEN_PEN(1u << LCD_FRONT6_PIN) |   				 //LCD_P10
		LCD_PEN_PEN(1u << LCD_FRONT7_PIN) | 				   //LCD_P11
		LCD_PEN_PEN(1u << LCD_FRONT1_PIN) | 				   //LCD_P17
		LCD_PEN_PEN(1u << LCD_BACKP3_PIN) | 				   //LCD_P18
		LCD_PEN_PEN(1u << LCD_BACKP2_PIN);  				   //LCD_P19
	LCD->PEN[1] = 
		LCD_PEN_PEN(1u << (LCD_FRONT0_PIN - 32u)) |    //LCD_P37
		LCD_PEN_PEN(1u << (LCD_FRONT5_PIN - 32u)) |    //LCD_P38
		LCD_PEN_PEN(1u << (LCD_BACKP0_PIN - 32u)) |    //LCD_P40
		LCD_PEN_PEN(1u << (LCD_BACKP1_PIN - 32u)) |    //LCD_P52
		LCD_PEN_PEN(1u << (LCD_FRONT4_PIN - 32u));     //LCD_P53

	//BPEN (Back Plane Enable)
		//for pins >= 32, set in LCD->BPEN[1] as LCD_BPEN_BPEN(Pin#-32)
	LCD->BPEN[0] = 
		LCD_BPEN_BPEN(1u << LCD_BACKP3_PIN) |   			 //LCD_P18  COM3
		LCD_BPEN_BPEN(1u << LCD_BACKP2_PIN);   				 //LCD_P19, COM2
 LCD->BPEN[1] = 
		LCD_BPEN_BPEN(1u << (LCD_BACKP0_PIN - 32u)) |  //LCD_P40, COM0
		LCD_BPEN_BPEN(1u << (LCD_BACKP1_PIN - 32u));   //LCD_P52, COM1
	
	//WF (Waveform)
		//konfiguracja polega na równomiernym rozlozeniu (w tym przypadku 4) faz na 8 bitach
	LCD->WF[0]  =
		LCD_WF_WF0(0x00) |      //WF Pin 0 Disabled
		LCD_WF_WF1(0x00) |      //WF Pin 1 Disabled
		LCD_WF_WF2(0x00) |      //WF Pin 2 Disabled
		LCD_WF_WF3(0x00) ;      //WF Pin 3 Disabled
	LCD->WF[1]  =
		LCD_WF_WF4(0x00) |      //WF Pin 4 Disabled
		LCD_WF_WF5(0x00) |      //WF Pin 5 Disabled
		LCD_WF_WF6(0x00) |      //WF Pin 6 Disabled
		LCD_WF_WF7(0x00) ;      //WF Pin 7 Off
	LCD->WF[2]  =
		LCD_WF_WF8(0x00) |      //WF Pin 8 Off
		LCD_WF_WF9(0x00) |      //WF Pin 9 Disabled
		LCD_WF_WF10(0x00)|      //WF Pin 10 Off
		LCD_WF_WF11(0x00);      //WF Pin 11 Off
	LCD->WF[3]  =
		LCD_WF_WF12(0x00)|      //WF Pin 12 Disabled
		LCD_WF_WF13(0x00)|      //WF Pin 13 Disabled
		LCD_WF_WF14(0x00)|      //WF Pin 14 Disabled
		LCD_WF_WF15(0x00);      //WF Pin 15 Disabled
	LCD->WF[4]  =
		LCD_WF_WF16(0x00)|      //WF Pin 16 Disabled
		LCD_WF_WF17(0x00)|      //WF Pin 17 Off
		LCD_WF_WF18(0x88)|      //WF Pin 18 (COM3) is enabled on Phases D and H
		LCD_WF_WF19(0x44);      //WF Pin 19 (COM2) is enabled on Phases C and G
	LCD->WF[5]  =
		LCD_WF_WF20(0x00)|      //WF Pin 20 Disabled
		LCD_WF_WF21(0x00)|      //WF Pin 21 Disabled
		LCD_WF_WF22(0x00)|      //WF Pin 22 Disabled
		LCD_WF_WF23(0x00);      //WF Pin 23 Disabled
	LCD->WF[6]  =
		LCD_WF_WF24(0x00)|      //WF Pin 24 Disabled
		LCD_WF_WF25(0x00)|      //WF Pin 25 Disabled
		LCD_WF_WF26(0x00)|      //WF Pin 26 Disabled
		LCD_WF_WF27(0x00);      //WF Pin 27 Disabled
	LCD->WF[7]  =
		LCD_WF_WF28(0x00)|      //WF Pin 28 Disabled
		LCD_WF_WF29(0x00)|      //WF Pin 29 Disabled
		LCD_WF_WF30(0x00)|      //WF Pin 30 Disabled
		LCD_WF_WF31(0x00);      //WF Pin 31 Disabled
	LCD->WF[8]  =
		LCD_WF_WF32(0x00)|      //WF Pin 32 Disabled
		LCD_WF_WF33(0x00)|      //WF Pin 33 Disabled
		LCD_WF_WF34(0x00)|      //WF Pin 34 Disabled
		LCD_WF_WF35(0x00);      //WF Pin 35 Disabled
	LCD->WF[9]  =
		LCD_WF_WF36(0x00)|      //WF Pin 36 Disabled
		LCD_WF_WF37(0x00)|      //WF Pin 37 Off
		LCD_WF_WF38(0x00)|      //WF Pin 38 Off
		LCD_WF_WF39(0x00);      //WF Pin 39 Disabled
	LCD->WF[10] =
		LCD_WF_WF40(0x11)|      //WF Pin 40 (COM0) is enabled on Phases A and E
		LCD_WF_WF41(0x00)|      //WF Pin 41 Disabled
		LCD_WF_WF42(0x00)|      //WF Pin 42 Disabled
		LCD_WF_WF43(0x00);      //WF Pin 43 Disabled
	LCD->WF[11] =
		LCD_WF_WF44(0x00)|      //WF Pin 44 Disabled
		LCD_WF_WF45(0x00)|      //WF Pin 45 Disabled
		LCD_WF_WF46(0x00)|      //WF Pin 46 Disabled
		LCD_WF_WF47(0x00);      //WF Pin 47 Disabled
	LCD->WF[12] =
		LCD_WF_WF48(0x00)|      //WF Pin 48 Disabled
		LCD_WF_WF49(0x00)|      //WF Pin 49 Disabled
		LCD_WF_WF50(0x00)|      //WF Pin 50 Disabled
		LCD_WF_WF51(0x00);      //WF Pin 51 Disabled
	LCD->WF[13] =
		LCD_WF_WF52(0x22)|      //WF Pin 52 (COM1) is enabled on Phases B and F
		LCD_WF_WF53(0x00)|      //WF Pin 53 Off
		LCD_WF_WF54(0x00)|      //WF Pin 54 Disabled
		LCD_WF_WF55(0x00);      //WF Pin 55 Disabled
	LCD->WF[14] =
		LCD_WF_WF56(0x00)|      //WF Pin 56 Disabled
		LCD_WF_WF57(0x00)|      //WF Pin 57 Disabled
		LCD_WF_WF58(0x00)|      //WF Pin 58 Disabled
		LCD_WF_WF59(0x00);      //WF Pin 59 Disabled
	LCD->WF[15] =
		LCD_WF_WF60(0x00)|      //WF Pin 60 Disabled
		LCD_WF_WF61(0x00)|      //WF Pin 61 Disabled
		LCD_WF_WF62(0x00)|      //WF Pin 62 Disabled
		LCD_WF_WF63(0x00);      //WF Pin 63 Disabled
	
	//koniec konfiguracji
	LCD->GCR &= ~LCD_GCR_PADSAFE_MASK; //clear PADSAFE to unlock LCD pins
	LCD->GCR |= LCD_GCR_LCDEN_MASK;		 //wlaczenie wyswietlacza
}

void setDigitLCD( uint8_t digit, uint8_t position ) {
	uint8_t H;
	
	if( position > 4 )
		errorLCD(LCD_ERROR_POSITION);
	position = (2*position) - 2;
	
	//seg H preservation
	H = LCD->WF8B[ LCD_Front_Pin[position+1] ] & LCD_SEG_H;
	
	if( digit == 0x00 ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			(LCD_SEG_D | LCD_SEG_E | LCD_SEG_F);
		LCD->WF8B[ LCD_Front_Pin[position+1] ] = 
			(LCD_SEG_A | LCD_SEG_B | LCD_SEG_C | H);
	} 
	else if( digit == 0x01 ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			LCD_CLEAR;
		LCD->WF8B[ LCD_Front_Pin[position+1] ] = 
			(LCD_SEG_B | LCD_SEG_C | H);
	}
	else if( digit == 0x02 ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			(LCD_SEG_D | LCD_SEG_E | LCD_SEG_G);
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			(LCD_SEG_A | LCD_SEG_B | H);
	}
	else if( digit == 0x03 ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			(LCD_SEG_D | LCD_SEG_G);
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			(LCD_SEG_A | LCD_SEG_B | LCD_SEG_C | H);
	}
	else if( digit == 0x04 ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			(LCD_SEG_F | LCD_SEG_G);
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			(LCD_SEG_B | LCD_SEG_C | H);
	}
	else if( digit == 0x05 ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			(LCD_SEG_D | LCD_SEG_F | LCD_SEG_G);
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			(LCD_SEG_A | LCD_SEG_C | H);
	}
	else if( digit == 0x06 ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			(LCD_SEG_D | LCD_SEG_E | LCD_SEG_F | LCD_SEG_G);
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			(LCD_SEG_A | LCD_SEG_C | H);
	}
	else if( digit == 0x07 ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			LCD_CLEAR;
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			(LCD_SEG_A | LCD_SEG_B | LCD_SEG_C | H);
	}
	else if( digit == 0x08 ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			(LCD_SEG_D | LCD_SEG_E | LCD_SEG_F | LCD_SEG_G);
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			(LCD_SEG_A | LCD_SEG_B | LCD_SEG_C | H);
	}
	else if( digit == 0x09 ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			(LCD_SEG_D | LCD_SEG_F | LCD_SEG_G);
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			(LCD_SEG_A | LCD_SEG_B | LCD_SEG_C | H);
	}
	else if( digit == 0x0A ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			(LCD_SEG_E | LCD_SEG_F | LCD_SEG_G);
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			(LCD_SEG_A | LCD_SEG_B | LCD_SEG_C | H);
	}
	else if( digit == 0x0B ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			(LCD_SEG_D | LCD_SEG_E | LCD_SEG_F | LCD_SEG_G);
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			LCD_SEG_C | H;
	}
	else if( digit == 0x0C ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			(LCD_SEG_D | LCD_SEG_E | LCD_SEG_F);
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			LCD_SEG_A | H;
	}
	else if( digit == 0x0D	)	{
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			(LCD_SEG_D | LCD_SEG_E | LCD_SEG_G);
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			LCD_SEG_B | LCD_SEG_C | H;
	}
	else if( digit == 0x0E ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			(LCD_SEG_D | LCD_SEG_E | LCD_SEG_F | LCD_SEG_G);
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			LCD_SEG_A | H;
	}
	else if( digit == 0x0F ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			(LCD_SEG_E | LCD_SEG_F | LCD_SEG_G);
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			LCD_SEG_A | H;
	}
	else if( digit == 0xFF ) {
		LCD->WF8B[ LCD_Front_Pin[position]   ] =
			LCD_CLEAR;
		LCD->WF8B[ LCD_Front_Pin[position+1] ] =
			LCD_CLEAR | H;
	}
	else
		errorLCD(LCD_ERROR_DIGIT_VALUE);
}

void setNumberLCD( uint32_t number, uint8_t base ) {
	uint8_t clear = base & LCD_CLEAR_LEADING_ZEROS;
	base &= ~LCD_CLEAR_LEADING_ZEROS;
	if( (base == 10u && number > 9999uL) ||
			(base == 2u && number > 15uL) ||
			(base == 8u && number > 4095uL) ||
			(base == 16u && number > 65535uL) ||
			(base != 10u && base != 2u && base != 8u && base != 16u) )
				errorLCD(LCD_ERROR_NUMBER);

	else {
		uint8_t i;
		for( i=4; i>=1; i-- ) {
			setDigitLCD( number % base, i );
			number /= base;
			//at least 1 digit has been displayed
			if( clear && number == 0 ) {
				i--; //get to next digit
				for( ; i>=1; i--)
					setDigitLCD( 0xFF, i);
				break;
			}
		}
	}
			
	return;
}

void setTimeLCD( timeStruct *time ) {
	uint8_t temp;
	
	//inkrementuj liczniki czasu
	if( ++(time->s) == 60 ) {
		time->s = 0;
		if( ++(time->m) == 60) {
			time->m = 0;
			time->h++;
			time->h %= 24;
		}
	}
	
	temp = time->m;
	setDigitLCD( temp % 10, 4 );
	temp /= 10;
	setDigitLCD( temp % 10, 3 );
	temp = time->h;
	setDigitLCD( temp % 10, 2 );
	temp /= 10;
	setDigitLCD( (temp) ? (temp % 10) : 0xFF, 1 ); //zabezpieczenie przed godzinami w stylu "08"
}


void errorLCD( uint8_t code ) {
	uint8_t position = 1;
	
	offDotLCD( LCD_MASK_DOT_ALL & ~LCD_MASK_DOT3 );
	onDotLCD( LCD_MASK_DOT3 );
	
	//position = 1;
	LCD->WF8B[ LCD_Front_Pin[((2*position)-2)] ] = (
		LCD_SEG_D |
		LCD_SEG_E |
		LCD_SEG_F |
		LCD_SEG_G);
	LCD->WF8B[ LCD_Front_Pin[((2*position)-1)] ] =
		LCD_SEG_A;
	
	position++;
	LCD->WF8B[ LCD_Front_Pin[((2*position)-2)] ] = (
		LCD_SEG_E |
		LCD_SEG_G);
	LCD->WF8B[ LCD_Front_Pin[((2*position)-1)] ] =
		LCD_CLEAR;
	
	position++;
	LCD->WF8B[ LCD_Front_Pin[((2*position)-2)] ] = (
		LCD_SEG_E |
		LCD_SEG_G);
	LCD->WF8B[ LCD_Front_Pin[((2*position)-1)] ] =
		LCD_CLEAR;
		
	position++;
	if( code <= 0x0F )
		setDigitLCD( code, position );
	else {
		LCD->WF8B[ LCD_Front_Pin[((2*position)-2)] ] =
			LCD_CLEAR;
		LCD->WF8B[ LCD_Front_Pin[((2*position)-1)] ] =
			LCD_CLEAR;
	}
}


void toggleDotLCD( uint8_t dot ) {
	if( dot & LCD_MASK_DOT1 )
		LCD->WF8B[LCD_FRONT1_PIN] ^= LCD_SEG_H;
	if( dot & LCD_MASK_DOT2 )
		LCD->WF8B[LCD_FRONT3_PIN] ^= LCD_SEG_H;
	if( dot & LCD_MASK_DOT3 )
		LCD->WF8B[LCD_FRONT5_PIN] ^= LCD_SEG_H;
	if( dot & LCD_MASK_COLON )
		LCD->WF8B[LCD_FRONT7_PIN] ^= LCD_SEG_H;
}

void onDotLCD( uint8_t dot ) {
	if( dot & LCD_MASK_DOT1 )
		LCD->WF8B[LCD_FRONT1_PIN] |= LCD_SEG_H;
	if( dot & LCD_MASK_DOT2 )
		LCD->WF8B[LCD_FRONT3_PIN] |= LCD_SEG_H;
	if( dot & LCD_MASK_DOT3 )
		LCD->WF8B[LCD_FRONT5_PIN] |= LCD_SEG_H;
	if( dot & LCD_MASK_COLON )
		LCD->WF8B[LCD_FRONT7_PIN] |= LCD_SEG_H;
}

void offDotLCD( uint8_t dot ) {
	if( dot & LCD_MASK_DOT1 )
		LCD->WF8B[LCD_FRONT1_PIN] &= ~LCD_SEG_H;
	if( dot & LCD_MASK_DOT2 )
		LCD->WF8B[LCD_FRONT3_PIN] &= ~LCD_SEG_H;
	if( dot & LCD_MASK_DOT3 )
		LCD->WF8B[LCD_FRONT5_PIN] &= ~LCD_SEG_H;
	if( dot & LCD_MASK_COLON )
		LCD->WF8B[LCD_FRONT7_PIN] &= ~LCD_SEG_H;
}

void clearLCD(void) {
	setDigitLCD( 0xFF, 1 );
	setDigitLCD( 0xFF, 2 );
	setDigitLCD( 0xFF, 3 );
	setDigitLCD( 0xFF, 4 );
	offDotLCD( LCD_MASK_DOT_ALL );
}
