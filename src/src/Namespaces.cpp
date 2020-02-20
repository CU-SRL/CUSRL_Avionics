/**
 * @file Namespaces.cpp
 * @brief The namespace source file for the CUSRL_Avionics Code Base
 * 
 * This source file defines all the namespaces and their member variables
 * 
 */

/***************************************************************/
/*                                                             */
/*                                                             */
/*                    CU-SRL AVIONICS TEAM                     */
/*                   Namespace Definitions                     */
/*                                                             */
/*                       Jason Popich                          */
/*                       Lyon Foster                           */
/*                       Carter Mak                            */
/*                       Giselle Koo                           */
/*                                                             */
/***************************************************************/

#include "yonics.hpp"


//  THE VARIABLES ARE DEFINED HERE
namespace INITS
{
    // ALL PIN ASSIGNMENTS ARE ON A BOARD BY BOARD CASE
    int speakerPin = 2;
    int highG_xPin = 33;
    int highG_yPin = 34;
    int highG_zPin = 35;

    DigitalIMU IMU = DigitalIMU(55,0x28);
    DigitalBAROM BAROM;
    AnalogIMU HIGHG = AnalogIMU(highG_xPin,highG_yPin,highG_zPin,true);
    BeepyBOI berp = BeepyBOI(speakerPin);

    DLLflash* flash;

    IMUdata imu_data;
    BAROMdata barom_data;
    ACCELdata accel_data;
};

namespace PROTOTHREADING
{
    int interval_IMU = 45;
    int interval_BAROM = 2000;
    int interval_ACCEL = 50;

    ThreadController thread_control = ThreadController();

    Thread* ThreadIMU = new Thread();
    Thread* ThreadBAROM = new Thread();
    Thread* ThreadACCEL = new Thread();

};

namespace I2C
{
    bool write_reg(uint8_t i2c, uint8_t addr, uint8_t val)
    {
    	Wire.beginTransmission(i2c);
    	Wire.write(addr);
    	Wire.write(val);
    	return Wire.endTransmission() == 0;
    }

    bool read_regs(uint8_t i2c, uint8_t addr, uint8_t *data, uint8_t num)
    {
    	Wire.beginTransmission(i2c);
    	Wire.write(addr);
    	if (Wire.endTransmission(false) != 0) return false;
    	Wire.requestFrom(i2c, num);
    	if (Wire.available() != num) return false;
    	while (num > 0) {
    		*data++ = Wire.read();
    		num--;
    	}
    	return true;
    }

    bool read_regs(uint8_t i2c, uint8_t *data, uint8_t num)
    {
    	Wire.requestFrom(i2c, num);
    	if (Wire.available() != num) return false;
    	while (num > 0) {
    		*data++ = Wire.read();
    		num--;
    	}
    	return true;
    }
};