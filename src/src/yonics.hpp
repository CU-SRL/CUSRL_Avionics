#ifndef _YONICS_HPP_
#define _YONICS_HPP_

//! HEADER FILES
/*!
*   All the main header files that the project uses are linked here.
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

// MACROS
/*
*   Some MACROS that need to be defined beforehand
*/
#define GPSSerial Serial3 /*!< Define the GPS hardware Serial Port */

struct ACCELdata {
    float x;
    float y;
    float z;
    uint32_t t;
};

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

struct BAROMdata {
    float pressure = 0;
    float altitude = 0;
    float temperature = 0;
    uint32_t t = 0;
};

class AnalogIMU {
    // Class to manage the Adafruit ADXL377 breakout board
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

class DigitalBAROM {
    private:
        Adafruit_MPL3115A2 BAROM = Adafruit_MPL3115A2();
    public:
        DigitalBAROM();
        bool begin();
        bool sample(BAROMdata* data);
};

class BeepyBOI {
    private:
        int pin;
        int errTone = 300;
        int lowTone = 220;
        int midTone = 440;
        int  hiTone = 880;
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
    extern int speakerPin;
    extern int highG_xPin;
    extern int highG_yPin;
    extern int highG_zPin;

    // CLASS INITIALIZATIONS
    extern DigitalIMU IMU;
    extern DigitalBAROM BAROM;
    extern AnalogIMU HIGHG;
    extern BeepyBOI berp;

    // POINTERS
    extern DLLflash* flash;

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