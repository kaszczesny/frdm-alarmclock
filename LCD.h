#ifndef LCD_h
#define LCD_h

#include "MKL46Z4.h"

typedef struct timeStruct {
	uint8_t s;
	uint8_t m;
	uint8_t h;
} timeStruct;

//liczba pinow na plaszczyznach
#define LCD_NUM_FRONT_PINS 8
#define LCD_NUM_BACKP_PINS 4

/* Pin_1 -> (position*2 - 1), Pin_2 -> position*2
 			Pin_1  Pin_2			     A
 COM0		D			 H				    __
 COM1		E			 C				 F | G|  B
 COM2		G			 B				   |--|
 COM3		F			 A				 E |__|. C
												       H 
												    D
*/
#define LCD_SEG_D 0x11	//segment D
#define LCD_SEG_E 0x22	//segment E
#define LCD_SEG_G 0x44	//segment G
#define LCD_SEG_F 0x88	//segment F
#define LCD_SEG_H 0x11	//segment H (kropka)
#define LCD_SEG_C 0x22	//segment C
#define LCD_SEG_B 0x44	//segment B
#define LCD_SEG_A 0x88	//segment A
#define LCD_CLEAR 0x00	//clear

//piny
#define LCD_FRONT0_PIN 37u
#define LCD_FRONT1_PIN 17u
#define LCD_FRONT2_PIN 7u
#define LCD_FRONT3_PIN 8u
#define LCD_FRONT4_PIN 53u
#define LCD_FRONT5_PIN 38u
#define LCD_FRONT6_PIN 10u
#define LCD_FRONT7_PIN 11u
#define LCD_BACKP0_PIN 40u
#define LCD_BACKP1_PIN 52u
#define LCD_BACKP2_PIN 19u
#define LCD_BACKP3_PIN 18u

#define LCD_MASK_DOT1  		(1u << 0u)
#define LCD_MASK_DOT2  		(1u << 1u)
#define LCD_MASK_DOT3  		(1u << 2u)
#define LCD_MASK_COLON 		(1u << 3u)
#define LCD_MASK_DOT_ALL 	(LCD_MASK_DOT1 | LCD_MASK_DOT2 | LCD_MASK_DOT3 | LCD_MASK_COLON)

#define LCD_CLEAR_LEADING_ZEROS  (1u << 0u)

#define LCD_ERROR_POSITION				0x00
#define LCD_ERROR_DIGIT_VALUE			0x01
#define LCD_ERROR_NUMBER					0x02


//inicjalizacja odpowiednich rejestrow
void initLCD(void);

//ustawienie konkretnej cyfry (funkcja raczej pomocnicza)
	//digit = 0xff - wyczyszczenie cyfry
void setDigitLCD( uint8_t digit, uint8_t position );

//wyswietlenie liczby o danej podstawie
	//jezeli base ma bit 1, oznacza to, ze nieznaczace zera maja byc wyczyszczone
void setNumberLCD( uint32_t number, uint8_t base );

//wyswietlanie i inkrementacja czasu (docelowo wywolywane przez RTC_Seconds_IRQHandler)
	//miganie dwukropka odbywa sie poza ta funkcja
void setTimeLCD( timeStruct *time );

//wyswietlenie informacji o bledzie
void errorLCD( uint8_t code );

//zmienia stan kropki/dwukropka na przeciwny (moze przyjac sume masek)
void toggleDotLCD( uint8_t dot );

//wlacza kropke/dwukropek (moze przyjac sume masek)
void onDotLCD( uint8_t dot );

//wylacza kropke/dwukropek (moze przyjac sume masek)
void offDotLCD( uint8_t dot );

//czysci caly ekran
void clearLCD(void);

#endif
