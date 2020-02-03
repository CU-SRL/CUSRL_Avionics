
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
#include <SPIMemory.h>
#include <SdFat.h>
#include "RFM96W.hpp"
#include "register.hpp"
#include <Eigen/Dense>
#include <math.h>

struct GPSdata  {
    float lat = 0;
    float lon = 0;
    float altitude = 0;
    float speed = 0;
    float angle = 0;
    float sat_num = 0;
    uint32_t t;
};

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

class SaveSD {
    private:
        bool running = false;
        SdFatSdio sd;
        File of;
        char foldername[10];

        void printIMU(IMUdata* data);
        void printBAROM(BAROMdata* data);
        void printACCEL(ACCELdata* data);
        void printGPS(GPSdata* data);

        bool openIMU();
        bool openBAROM();
        bool openACCEL();
        bool openGPS();

    public:
        SaveSD();

        bool initFolder();
        
        bool sampleIMU(IMUdata* data);
        bool sampleBAROM(BAROMdata* data);
        bool sampleACCEL(ACCELdata* data);
        bool sampleGPS(GPSdata* data);
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
    public:
        DigitalBAROM();
        bool begin();
        bool sample(BAROMdata* data);
};

class DigitalGPS {
    private:
    public:
        Adafruit_GPS* GPS;
        HardwareSerial* GPSSerial;

        DigitalGPS(HardwareSerial *ser);

        void dummyPrint();
        void initGPS();
        void eraseLOCUS();
        void GPSData_dump_setup();
        void refresh_GPSData(bool GPSECHO);
        void pullGPSFlashData();
        void pullRawGPS(/*GPSdata* data*/);
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

class pyroPorts {
    private:
        int fireAlt;

        int pyro1 = 24;
        int pyro2 = 25;
        int pyro3 = 26;
        int pyro4 = 27;
        // ^^^^^^^^^^^^ up to date with "teensy" page in pin allocations
    
        BAROMdata* BAROM_ptr;
    public:
        pyroPorts(int fireAlt, BAROMdata* BAROM_ptr);
        void fireAtApogee();
        void fireAtAlt();
};

class intAndFilter{
    private:
        // some of the basic stuff for rk4
        // this one only integrates for velocity in the z_hat direction bc it's only need for drogue
        //deployment at apogee
        // double zVel_n = 0; //velocity in the z_hat direction
        // double dzVel_dtminus //the last accel data point
        // double dzVel_dt; //discrete acceleration in the z_hat direction at the instant time_n
        // double zVel_0 = 0; // our initial velocity in the z_hat direction, is 0 at launch
        // double zVel_nplus; // the velocity in the z_hat direction at time_nplus

        // double zTheta_nplus; //cos of this angle is the direction of the rocket relative to a flat earth
       
        double a_x = 0;
        double a_y = 0;
        double a_z = 0;

        double v_x = 0;
        double v_y = 0;
        double v_z = 0;

        double v_xB = 0;
        double v_yB = 0;
        double v_zB = 0;

        double alpha_x = 0;
        double alpha_y = 0;
        double alpha_z = 0;

        double omega_x = 0;
        double omega_y = 0;
        double omega_z = 0;

        double theta_x = 0;
        double theta_y = 0;
        double theta_z = 0;

        double g = 9.81; // this changes to appogee--need look up table

        uint32_t t_minus = 0; //one h ago
        uint32_t t_n = 0; //current time
       
        
        IMUdata* IMU_ptr = nullptr;
   
    public:
        // intAndFilter();
        intAndFilter(IMUdata* IMU_ptr);
        double integrate();

};

static bool write_reg(uint8_t i2c, uint8_t addr, uint8_t val)
{
	Wire.beginTransmission(i2c);
	Wire.write(addr);
	Wire.write(val);
	return Wire.endTransmission() == 0;
}

static bool read_regs(uint8_t i2c, uint8_t addr, uint8_t *data, uint8_t num)
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

static bool read_regs(uint8_t i2c, uint8_t *data, uint8_t num)
{
	Wire.requestFrom(i2c, num);
	if (Wire.available() != num) return false;
	while (num > 0) {
		*data++ = Wire.read();
		num--;
	}
	return true;
}