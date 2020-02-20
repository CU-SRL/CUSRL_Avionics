/**
 * @file yonics.hpp
 * @brief The main header file for the CUSRL_Avionics Code Base
 */
#ifndef _YONICS_HPP_
#define _YONICS_HPP_

// HEADER FILES
/*
*   All the header files that the project uses are linked here.
*   Libraries, Sensors, Etc...
*/
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_GPS.h>
#include "Thread.h"
#include <ThreadController.h>
#include <utility/imumaths.h>
#include <SdFat.h>
#include "DLLflash.hpp"
#include "register.hpp"

//! HIGH-G Accelerometer Struct
/*!
*   This structs holds the ADXL377 sample at a point in time to be stored and processed. 
*/
struct ACCELdata {
    float x;
    float y;
    float z;
    uint32_t t;
};

//! IMU Struct
/*!
*   This structs holds the BNO055 sample at a point in time to be stored and processed.
*/
struct IMUdata {
    double orient_euler[3] = {0,0,0};
    double gyro_fused[3] = {0,0,0};
    double accel_fused[3] = {0,0,0};
    double accel_raw[3] = {0,0,0};
    double gyro_raw[3] = {0,0,0};
    double magnetometer[3] = {0,0,0};
    double orient_quat[4] = {0,0,0,0}; // {w,x,y,z}
    uint32_t t = 0;
};

//! Barometer Struct
/*!
*   This structs holds the MPL3115A2 sample at a point in time to be stored and processed.
*/
struct BAROMdata {
    float pressure = 0;
    float altitude = 0;
    float temperature = 0;
    uint32_t t = 0;
};

//! ADXL377 High-G Accelerometer Class
/*!
*   Class to manage the Adafruit ADXL377 High-G Accelerometer breakout board
*/
class AnalogIMU {
    private:
        int xPin, yPin, zPin;
        int bitDepth;
        int offset;
        float ratio;
        int maxG = 200;

        void init();
        float formatVal(int rawVal);
    public:
        AnalogIMU();
        AnalogIMU(int xPin, int yPin, int zPin);
        AnalogIMU(int xPin, int yPin, int zPin, bool highBitDepth);
        void sample(ACCELdata* data);
};

//! BNO055 IMU Class
/*!
*   Class to manage the Adafruit BNO055 Absolute Orientation IMU Fusion breakout board
*/
class DigitalIMU {
    private:
        Adafruit_BNO055 board;
        sensors_event_t event;
        imu::Quaternion quat;
        imu::Vector<3> accel;
    public:
        DigitalIMU();
        DigitalIMU(int32_t sensorID, uint8_t address);
        bool begin();
        void sample(IMUdata* data);
};

//! MPL3115A2 Barometer Class
/*!
*   Class to manage the MPL3115A2 I2C breakout board
*/
class DigitalBAROM {
    private:
    public:
        DigitalBAROM();
        bool begin();
        bool sample(BAROMdata* data);
};

//! Piezo Buzzer Class
/*!
*   This class handles all the Piezo Buzzer interactions.
*   The Buzzer will sound certain noises to indicate errors, startup, etc...
*/
class BeepyBOI {
    private:
        int pin; /*<! The pin that the piezo buzzer is connected to. */

        // Some predefined tones for simplicity in the code...
        // Define all the tones here.
        int errTone = 300; /*<! Predefined Error Tone */
        int lowTone = 220; /*<! Predefined Low Tone */
        int midTone = 440; /*<! Predefined Mid Tone */
        int  hiTone = 880; /*<! Predefined High Tone */
    public:
        BeepyBOI();
        BeepyBOI(int pin);
        void hello();
        void error();
        void countdown(int s);
        void lowBeep();
        void midBeep();
        void  hiBeep();
        void bombBeep();
};

/*!
*   In this namespace the I2C drivers are declared and then defined for use with the I2C protocol
*   In order to achieve I2C communication, the Arduino Wire library is used to simplify the complexity of the functions.
*/
namespace I2C
{
    extern bool write_reg(uint8_t i2c, uint8_t addr, uint8_t val);

    extern bool read_regs(uint8_t i2c, uint8_t addr, uint8_t *data, uint8_t num);

    extern bool read_regs(uint8_t i2c, uint8_t *data, uint8_t num);
};

/*!
*   All pointers and objects that are required in the main file are put
*   within the INITS namespace to avoid global variables and their implications
*/
namespace INITS
{
    // PIN ASSIGNMENTS
    extern int speakerPin; /*!< The Piezo Buzzer pin */
    extern int highG_xPin; /*!< The High-G Accelerometer X Pin Assignment */
    extern int highG_yPin; /*!< The High-G Accelerometer Y Pin Assignment */
    extern int highG_zPin; /*!< The High-G Accelerometer Z Pin Assignment */

    // CLASS INITIALIZATIONS
    extern DigitalIMU IMU; /*!< The DigitalIMU class object, that will be initialized for the BNO055 IMU */
    extern DigitalBAROM BAROM; /*!< The DigitalBAROM class object, that will be initialized for the MPL3115A2 Barometer*/
    extern AnalogIMU HIGHG; /*!< The AnalogIMU class object, that will be initialized for the ADXL377 High-G Accelerometer */
    extern BeepyBOI berp; /*!< The BeepyBOI class object, that will be initialized for the Piezo Buzzer */

    // POINTERS
    extern DLLflash* flash; /*!< The DLLflash pointer that will point to the DLLflash class instance */

    // DATA STRUCTS
    extern IMUdata imu_data; /*!< The struct IMUData object, or Instance, that holds all IMU data for processing and transmission */
    extern BAROMdata barom_data; /*!< The struct BAROMdata object, or Instance, that holds all BAROM data for processing and transmission */
    extern ACCELdata accel_data; /*!< The struct ACCELdata object, or Instance, that holds all the HIGHG Accelerometer data for processing and transmission */
};

/*!
*   The protothreading system is implemented by the ArduinoThread library
*
*   The Teensy 3.6 / 4.0 micro-controllers are one core, one thread therefore true asynchronous operation cannot be accomplished
*   This is unfortunate because optimally all sampling of the data should be done at the same time
*   Therefore in order to work around this limitation, a form of threading was introduced that approaches asynchronous operation
*   without actually achieving it, aka protothreading
*    
*   The way it works is essentially by having a(n) overall controller (i.e. ThreadController class) that manages the timing of all functions
*   you want to run
*   Whenever a function has reached the time it needs to be called again the ThreadController will call the the function and interrupt
*   whatever is currently running
*
*   The pro about this is that it also allows us to deal with different intervals that sensors or components require
*   Such as one component needing more time over the other
*   Every process interval is based off of the datasheet and its recommendations on sampling time
*
*/
namespace PROTOTHREADING
{
    // PROTOTHREADING TIME INTERVALS
    /*
    *   Defining the time intervals (in milliseconds) at which to call the "threads"
    *   
    *   Every important task (i.e. sampling, writing to flash, RF, etc...) has a "thread"
    *   and their intervals are defined here.
    */  
    extern int interval_IMU; /*!< The interval at which the IMU will refresh */
    extern int interval_BAROM; /*!< The interval at which the Barometer will refresh */
    extern int interval_ACCEL; /*!< The inverval at which the High-G Accelerometer will refresh */

    // PROTOTHREADING Declaration and Definitions
    /*
    *   All the required thread objects and pointers are declared and defined here
    */
    extern ThreadController thread_control; /*!< thread_control is the overarching ThreadController that handles all the timing */

    // Every thread is a pointer that is pointing to an object, or instance, of the Thread class initiated dynimically in order to use them in any scope
    extern Thread* ThreadIMU; /*!< The pointer that will point to the instance of the Thread for IMU */
    extern Thread* ThreadBAROM; /*!< The pointer that will point to the instance of the Thread for the Barometeer */
    extern Thread* ThreadACCEL; /*!< The pointer that will point to the instance of the Thread for the High-G Accelerometer */
};

#endif