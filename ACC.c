#include "ACC.h"

void enableACC(void) {
	//z jakichs przyczyn akcelerometr czasem nie dziala dobrze,
		//jesli nie wpisze mu sie rejestrow dwukrotnie
	writeRegACC( MMA8451_CTRL_REG2,    0x40 ); //RESET
	writeRegACC( MMA8451_CTRL_REG2,    0x40 ); //RESET
	
	writeRegACC( MMA8451_CTRL_REG1,    0x00 ); //STANDBY
	writeRegACC( MMA8451_CTRL_REG1,    0x00 ); //STANDBY
	
	writeRegACC( MMA8451_CTRL_REG1,    0x20 ); //Output Data Rate = 50 Hz
	writeRegACC( MMA8451_CTRL_REG1,    0x20 ); //Output Data Rate = 50 Hz
	
	writeRegACC( MMA8451_CTRL_REG2,    0x03 ); //Low Power Oversampling
	writeRegACC( MMA8451_CTRL_REG2,    0x03 ); //Low Power Oversampling
	
	writeRegACC( MMA8451_XYZ_DATA_CFG, 0x02 ); //8g scale range
	writeRegACC( MMA8451_XYZ_DATA_CFG, 0x02 ); //8g scale range
	
	writeRegACC( MMA8451_CTRL_REG1,    0x21 ); //ACTIVE
	writeRegACC( MMA8451_CTRL_REG1,    0x21 ); //ACTIVE
	
	writeRegACC( MMA8451_CTRL_REG2,    0x03 ); //Low Power Oversampling
	writeRegACC( MMA8451_CTRL_REG2,    0x03 ); //Low Power Oversampling
	
	writeRegACC( MMA8451_XYZ_DATA_CFG, 0x02 ); //8g scale range
	writeRegACC( MMA8451_XYZ_DATA_CFG, 0x02 ); //8g scale range
}

void disableACC(void) {
	uint8_t t = readRegACC( MMA8451_CTRL_REG1 );
	writeRegACC( MMA8451_CTRL_REG1, t & 0xFE ); //STANDBY
	writeRegACC( MMA8451_CTRL_REG1, t & 0xFE ); //STANDBY
}

uint8_t readRegACC(uint8_t addr) {
  uint8_t data;

  enableI2C0();

  startI2C0();                         // S
  writeByteI2C0(MMA8451Q_ADDR_WRITE);  // Slave address + W
  waitI2C0();
  writeByteI2C0(addr);                 // Select register to read
  waitI2C0();
  restartI2C0();                       // RS
  writeByteI2C0(MMA8451Q_ADDR_READ);   // Slave address + R
  waitI2C0();
  rxModeI2C0();                        // Mode: Rx
  disableAckI2C0();                    // NACK
  data = readByteI2C0();               // Clear Data register, also initiate read sequence
  waitI2C0();
  stopI2C0();                          // P
  data = readByteI2C0();               // Get value

  disableI2C0();

  return data;
}

void writeRegACC(uint8_t addr, uint8_t data) {
  enableI2C0();

  startI2C0();                         // S
  writeByteI2C0(MMA8451Q_ADDR_WRITE);  // Slave address + W
  waitI2C0();
  writeByteI2C0(addr);                 // Select register for write
  waitI2C0();
  writeByteI2C0(data);                 // Data
  waitI2C0();
  stopI2C0();                          // P

  disableI2C0();
}

int16_t readAxisValue(Axis a)
{
  uint8_t addr;
  int16_t value;

  switch(a) {
    case X:
      addr = MMA8451_OUT_X_MSB;
      break;
		
    case Y:
      addr = MMA8451_OUT_Y_MSB;
      break;
    
    case Z:
      addr = MMA8451_OUT_Z_MSB;
      break;
    
    default:
      break;
  }

  value = (readRegACC(addr) << 8);
  value += (readRegACC(addr+1));
	value /= 4; //wyskalowanie

  return value;
}

uint32_t getVectorACC(void) {
	int16_t t;
	uint32_t v = 0;
	
	t = readAxisValue(X);
	v += (uint32_t) t*t;
	
	t = readAxisValue(Y);
	v += (uint32_t) t*t;
	
	t = readAxisValue(Z);
	v += (uint32_t) t*t;
	
	return _sqrt( v/3 ); //Root Mean Square
}

//na podstawie: http://www.finesse.demon.co.uk/steven/sqrt.html
#define iter1(N) \
    t = root + (1 << (N)); \
    if (n >= t << (N)) { \
			n -= t << (N);   \
      root |= 2 << (N); \
    }

uint32_t _sqrt (uint32_t n) {
    uint32_t root = 0, t;
    iter1 (15);    iter1 (14);    iter1 (13);    iter1 (12);
    iter1 (11);    iter1 (10);    iter1 ( 9);    iter1 ( 8);
    iter1 ( 7);    iter1 ( 6);    iter1 ( 5);    iter1 ( 4);
    iter1 ( 3);    iter1 ( 2);    iter1 ( 1);    iter1 ( 0);
    return root >> 1;
}
