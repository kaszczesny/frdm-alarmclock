#ifndef ACC_h
#define ACC_h

#include "MKL46Z4.h"
#include "globals.h"
#include "I2C.h"

//na podstawie: http://www.kinetis.pl/node/158

#define MMA8451_OUT_X_MSB     0x01
#define MMA8451_OUT_X_LSB     0x02
#define MMA8451_OUT_Y_MSB     0x03
#define MMA8451_OUT_Y_LSB     0x04
#define MMA8451_OUT_Z_MSB     0x05
#define MMA8451_OUT_Z_LSB     0x06
#define MMA8451_WHO_AM_I      0x0D
#define MMA8451_XYZ_DATA_CFG  0x0E
#define MMA8451_CTRL_REG1     0x2A
#define MMA8451_CTRL_REG2     0x2B

#define MMA8451Q_ADDR_WRITE   0x3A//0x38
#define MMA8451Q_ADDR_READ    0x3B//0x39

typedef enum {
  X,
  Y,
  Z
} Axis;

//wlaczenie i ustawienie ACC
void enableACC(void);

//wylaczenie ACC
void disableACC(void);

//odczyt bajtu z rejestru
uint8_t readRegACC(uint8_t addr);

//zapis bajtu do rejestru
void writeRegACC(uint8_t addr, uint8_t data);

//odczyt przyspieszenia z jednej osi (-8g:8g --> -8192:8191)
int16_t readAxisValue(Axis a);

//odczyt przyspieszen na wszystkich osiach i zwrocenie ich RMS: 0-8192; w praktyce od 591 (bo grawitacja) do ~6000 (na jednej osi nawet ~4500)
uint32_t getVectorACC(void);

//szybka funkcja sqrt na int
uint32_t _sqrt (uint32_t n);

#endif
