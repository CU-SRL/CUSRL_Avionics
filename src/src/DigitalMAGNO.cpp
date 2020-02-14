#include "yonics.hpp"


/*
#define AK8963N_HXL                  0x03 // X-axis Data <7:0>
#define AK8963N_HXH                  0x04 // X-axis Data <15:8>
#define AK8963N_HYL                  0x05 // Y-axis Data <7:0>
#define AK8963N_HYH                  0x06 // Y-axis Data <15:8>
#define AK8963N_HZL                  0x07 // Z-axis Data <7:0>
#define AK8963N_HZH                  0x08 // Z-axis Data <15:8>
*/

bool DigitalMAGNO::begin() 
{
    const uint8_t i2c_addr=AK8963N_ADDRESS_1;
    // This device on only has an I2C disable feature. Device is disabled if 0x1B is written to it
    uint8_t b, b1, b2;

	// reading the device ID
	if (!read_regs(i2c_addr, AK8963N_WAI, &b, 1)) return false;
	Serial.println (b);

	// reading the device info
	if (!read_regs(i2c_addr, AK8963N_INFO, &b1, 1)) return false;
	Serial.println(b1);

	// setting the mode to continuous measurment mode 2
	if (!write_reg(i2c_addr, AK8963N_CNTL1, 0x16)) return false;
	if (!read_regs (i2c_addr, AK8963N_CNTL1, &b2, 1)) return false;
	Serial.println(b2);

	return true;
}

