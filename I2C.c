#include "I2C.h"

void initI2C0(void){
  SIM->SCGC5  |= SIM_SCGC5_PORTE_MASK;  // Wlaczenie sygnalu taktujacego dla portu E
  SIM->SCGC4  |= SIM_SCGC4_I2C0_MASK;   // Wlaczenie sygnalu taktujacego dla modulu I2C0
  
  PORTE->PCR[24] = //PTE24
		PORT_PCR_MUX(5) |    // 5 -> I2C0_SCL
    PORT_PCR_DSE_MASK;   // High drive strength
  
	PORTE->PCR[25] =
		PORT_PCR_MUX(5) |    // 5 -> I2C0_SDA
    PORT_PCR_DSE_MASK;   // High drive strength

  I2C0->F = //baudrate = bus/(mul*div) = 8e6/(1*1024) ~ 8kHz 
		I2C_F_MULT(0u) | //mul = 1
		I2C_F_ICR(0x33); //divider = 1024
}

void startI2C0(void) {
  I2C0->C1 |=
		I2C_C1_TX_MASK | //Transmit
		I2C_C1_MST_MASK; //Master mode
}

void restartI2C0(void) {
  I2C0->C1 |= I2C_C1_RSTA_MASK; //Repeat start
}

void stopI2C0(void) {
  I2C0->C1 &=
		~(I2C_C1_MST_MASK | //Slave mode
			I2C_C1_TX_MASK ); //Recieve
}

void writeByteI2C0(uint8_t data) {
  I2C0->D = data;                          // Zapis danych do wyslania
}

uint8_t waitI2C0 (void) {
  while( (I2C0->S & I2C_S_IICIF_MASK) == 0 ); //wait for interrupt

  // Check if Recieve ACK signal was detected, if not return 0
  if( (I2C0->S & I2C_S_RXAK_MASK) != 0 ) {
    return 0u;                             // Odebrano NACK
  }

  I2C0->S |= I2C_S_IICIF_MASK;             // Wyzerowanie flagi IICIF
  return 1u;
}

uint8_t readByteI2C0(void){
  return I2C0->D;                          // Pobranie przyjetych danych
}

void enableI2C0(void) {
  I2C0->C1 |= I2C_C1_IICEN_MASK;           // Wlaczenie I2C
}

void disableI2C0(void) {
  I2C0->C1 &= ~I2C_C1_IICEN_MASK;          // Wylaczenie I2C
}

void masterModeI2C0(void) {
  I2C0->C1 |= I2C_C1_MST_MASK;             // I2C w tryb Master
}

void slaveModeI2C0(void) {
  I2C0->C1 &= ~I2C_C1_MST_MASK;            // I2C w tryb Slave
}

void rxModeI2C0(void) {
  I2C0->C1 &= ~I2C_C1_TX_MASK;             // I2C w tryb odbioru
}

void txModeI2C0(void) {
  I2C0->C1 |= I2C_C1_TX_MASK;              // I2C w tryb nadawania
}

void disableAckI2C0(void) {
  I2C0->C1 |= I2C_C1_TXAK_MASK;            // Jezeli modul jest w trybie Rx: zostanie wyslany NACK
}
