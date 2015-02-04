#ifndef I2C_h
#define I2C_h

#include "MKL46Z4.h"
#include "globals.h"

//na podstawie: http://www.kinetis.pl/node/158

//inicjalizacja odpowiednich rejestrow
void initI2C0(void);

//tryb nadawania + master
void startI2C0(void);

//repeated START condition
void restartI2C0(void);

//tryb odbierania + slave
void stopI2C0(void);

//zapis danych do wyslania
void writeByteI2C0( uint8_t data );

//oczekiwanie na przerwanie (odbior danych)
uint8_t waitI2C0(void);

//pobieranie przyjetych danych
uint8_t readByteI2C0(void);

//wlaczenie modulu I2C0
void enableI2C0(void);

//wylaczenie moduli I2C0
void disableI2C0(void);

//tryb master
void masterModeI2C0(void);

//tryb slave
void slaveModeI2C0(void);

//tryb nadawania
void rxModeI2C0(void);

//tryb odbierania
void txModeI2C0(void);

//Jezeli modul jest w trybie Rx: zostanie wyslany NACK
void disableAckI2C0(void);

#endif
