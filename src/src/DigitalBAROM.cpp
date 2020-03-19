#include "yonics.hpp"

DigitalBAROM::DigitalBAROM() {}

bool DigitalBAROM::begin() 
{
    const uint8_t i2c_addr=MPL3115_I2C_ADDR;
    uint8_t b;

	if (!I2C::read_regs(i2c_addr, MPL3115_WHO_AM_I, &b, 1)) return false;
	if (b != 0xC4) return false;

	// place into standby mode
	if (!I2C::write_reg(i2c_addr, MPL3115_CTRL_REG1, 0)) return false;

    // switch to 34ms
	if (!I2C::write_reg(i2c_addr, MPL3115_CTRL_REG1, 0x98)) return false;

	// switch to active, set altimeter mode, set polling mode
	if (!I2C::write_reg(i2c_addr, MPL3115_CTRL_REG1, 0xB9)) return false;

	// enable events
	if (!I2C::write_reg(i2c_addr, MPL3115_PT_DATA_CFG, 0x07)) return false;
	return true;
}

bool DigitalBAROM::sample(BAROMdata* data) {

    static elapsedMicros usec_since;
	static int32_t usec_history = 980000;
	const uint8_t i2c_addr = MPL3115_I2C_ADDR;
	uint8_t buf[6];

    // KEEPS TIME BECAUSE MPL3115A2 times out after 512ms
	int32_t usec = usec_since;
	if (usec + 500 < usec_history) return false;

	if (!I2C::read_regs(i2c_addr, 0x00, buf, 1)) return false;
	if (buf[0] == 0) return false;

	// GRAB DATA and stick into buffer
	if (!I2C::read_regs(i2c_addr, buf, 6)) return false;

    // Updating time.
	usec_since -= usec;
	int diff = (usec - usec_history) >> 3;
	if (diff < -1000) diff = -1000;
	else if (diff > 1000) diff = 1000;
	usec_history += diff;

    // Get altitude from buffer and stick into altitude in struct
	int32_t a = ((uint32_t)buf[1] << 12) | ((uint16_t)buf[2] << 4) | (buf[3] << 4);
	if (a & 0x00080000) a |= 0xFFF00000;
	data->altitude = a;

    // Get temperature from buffer and stick in temperature in struct
	data->temperature = (int16_t)((buf[4] << 8) | buf[5]);

    Serial.print(data->altitude);
    Serial.print(" ");
    Serial.println(data->temperature);

    data->t = millis();

	return true;
}