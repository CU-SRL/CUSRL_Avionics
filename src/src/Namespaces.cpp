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

// INITS Namespace Definitions
namespace INITS
{
    // ALL PIN ASSIGNMENTS ARE ON A BOARD BY BOARD CASE
    int speakerPin = 2;     // Board speakerPin definition
    int highG_xPin = 33;    // Board highG_xPin defintion
    int highG_yPin = 34;    // Board highG_yPin defintion
    int highG_zPin = 35;    // Board highG_zPin defintion

    DigitalIMU IMU = DigitalIMU(55,0x28);                                   // Define the DigitalIMU object and set it equal to the DigitalIMU instance
    DigitalBAROM BAROM;                                                     // Create the DigitalBAROM class object
    HIGHG_ACCEL HIGHG = HIGHG_ACCEL(highG_xPin,highG_yPin,highG_zPin,true); // Define the AnalogIMU object and set it equal to the AnalogIMU instance
    BeepyBOI berp = BeepyBOI(speakerPin);                                   // Define the BeepyBOI object and set it equal to the BeepBOI instance
    
    DLLflash* flash = nullptr;                                              // Define the DLLflash object and set it equal to nullptr b/c it doesn't point to anything yet
                                                
    IMUdata imu_data;                                                       // Create the IMUData struct object
    BAROMdata barom_data;                                                   // Create the BAROMdata struct object
    ACCELdata accel_data;                                                   // Create the ACCELdata struct object
};

// PROTOTHREADING Namespace Definitions
namespace PROTOTHREADING
{
    int interval_IMU = 45;      // Define the interval at which to sample the IMU in milliseconds
    int interval_BAROM = 2000;  // Define the interval at which to sample the BAROM in milliseconds
    int interval_ACCEL = 50;    // Define the interval at which to sample the High-G ACCEL in milliseconds

    ThreadController thread_control = ThreadController(); // Create a new ThreadController instance for the ThreadController and set thread_control pointer equal to it

    Thread* ThreadIMU = new Thread();       // Create a new Thread instance for ThreadIMU and set ThreadIMU pointer equal to it
    Thread* ThreadBAROM = new Thread();     // Create a new Thread instance for ThreadBAROM and set ThreadBAROM pointer equal to it
    Thread* ThreadACCEL = new Thread();     // Create a new Thread instance for ThreadACCEL and set ThreadACCEL pointer equal to it
};

// I2C Namespace Definitions
namespace I2C
{
    // I2C - Write to Registry function
    bool write_reg(uint8_t i2c, uint8_t addr, uint8_t val)
    {
    	Wire.beginTransmission(i2c);            // Signal beginning of I2C transmission at i2c device
    	Wire.write(addr);                       // Place the register address we want to write to in the transmission buffer
    	Wire.write(val);                        // Place the value we want written in the transmission buffer
    	return Wire.endTransmission() == 0;     // Executes the buffer and signals end of MASTER transmission
    }

    // I2C - First Read from Registry function
    bool read_regs(uint8_t i2c, uint8_t addr, uint8_t *data, uint8_t num)
    {
    	Wire.beginTransmission(i2c);            // Signal beginning of I2C transmission at i2c device
    	Wire.write(addr);                       // Place the register address we want to read from in the transmission buffer
    	if (Wire.endTransmission(false) != 0)   // Execute the buffer and but DO NOT signal end of transmission because we want to read from the device
        {
            return false;                       // If failed, return false
        }
    	Wire.requestFrom(i2c, num);             // Request, from the address we specified above, a certain amount of bytes
    	if (Wire.available() != num)            // Make sure we have that amount of bytes available to read
        {
            return false;                       // If we dont have that amount of bytes, return false
        }
    	while (num > 0)
        {
    		*data++ = Wire.read();              // Read every byte and place into data buffer
    		num--;                              // Reduce by one num of bytes left to read
    	}
    	return true;                            // Return Success after reading
    }

    // I2C - Second Read from Registry function
    bool read_regs(uint8_t i2c, uint8_t *data, uint8_t num)
    {
    	Wire.requestFrom(i2c, num);             // Request, from the i2c device, a certain amount of bytes
    	if (Wire.available() != num)            // Make sure we have that amount of bytes available to read
        {       
            return false;                       // If we dont have that amount of bytes, return false
        }       
    	while (num > 0)     
        {       
    		*data++ = Wire.read();              // Read every byte and place into data buffer
    		num--;                              // Reduce by one num of bytes left to read
    	}       
    	return true;                            // Return Success after reading
    }
};