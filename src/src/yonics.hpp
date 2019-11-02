
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

struct GPSdata  {
    float lat = 0;
    float lon = 0;
    float altitude = 0;
    float speed = 0;
    float angle = 0;
    float sat_num = 0;
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

struct SampleTypes {
    int size = 0; // Size of one sample, in bytes
    int nSamples = 0; // Number of samples stored on chip
    uint32_t start_addr = 0; // Start address of this type's allocated memory
    void* data = NULL;
    float f = 0;
};

class FlashOp {
    // Class to manage saving data to and reading data from the flash chip

    private:
        SPIFlash* flash = NULL;

        // IMU Vars
        uint32_t addr_start_IMU = 0;
        uint8_t size_IMU = 0;
        uint32_t nSamples_IMU = 0;
        float freq_IMU = 0;

        // BAROM Vars
        uint32_t addr_start_BAROM = 0;
        uint8_t size_BAROM = 0;
        uint32_t nSamples_BAROM = 0;
        float freq_BAROM = 0;

        // ACCEL Vars
        uint32_t addr_start_ACCEL = 0;
        uint8_t size_ACCEL = 0;
        uint32_t nSamples_ACCEL = 0;
        float freq_ACCEL = 0;

        // GPS Vars
        uint32_t addr_start_GPS = 0;
        uint8_t size_GPS = 0;
        uint32_t nSamples_GPS = 0;
        float freq_GPS = 0;

    public:
        // init
        FlashOp();
        FlashOp(SPIFlash* flash);
        void addWP(int pin);

        // Data size initiation
        bool setIMU(uint8_t size, float frequency);
        bool setBAROM(uint8_t size, float frequency);
        bool setACCEL(uint8_t size, float frequency);

        // Reading
        bool readIMU(IMUdata* data, int idx);
        bool readBAROM(BAROMdata* data, int idx);
        bool readACCEL(ACCELdata* data, int idx);

        // Writing
        bool startWriting();
        bool writeIMU(IMUdata* data);
        bool writeBAROM(BAROMdata* data);
        bool writeACCEL(ACCELdata* data);

};

class SaveSD {
    private:
        bool running = false;
        SdFatSdio sd;
        File of;
        FlashOp* flash = NULL;

        IMUdata tempIMU;
        BAROMdata tempBAROM;
        ACCELdata tempACCEL;
        // GPSdata tempGPS;

        void printIMU();
        void printBAROM();
        void printACCEL();
        // void printGPS();
        // bool openFile();
    public:
        SaveSD();
        bool savenow();
        bool addFlashOp(FlashOp* flash);
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
        void sample(IMUdata* data, RFM96W_Client *client);
        
};

class DigitalBAROM {
    private:
        Adafruit_MPL3115A2 BAROM = Adafruit_MPL3115A2();
    public:
        DigitalBAROM();
        bool begin();
        void sample(BAROMdata* data);
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
