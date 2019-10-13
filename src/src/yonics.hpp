
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

struct GPSdata  {
    float lat = 0;
    float lon = 0;
    float altitude = 0;
    float speed = 0;
    float angle = 0;
    float sat_num = 0;
};

struct Acceldata {
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
        SdFatSdio sd;
        File of;
        SPIFlash* flash = NULL;
        void printIMU(uint32_t imuDataSize);
        void printBAROM(uint32_t baromDataSize, uint32_t read_addr_BAROM);
        bool openFile();
    public:
        SaveSD();
        bool savenow(uint32_t imuDataSize, uint32_t baromDataSize);
        bool addFlash(SPIFlash* flash);
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
        void sample(Acceldata* data);
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
        void sample(BAROMdata* data);
};

class DigitalGPS {
    private:
        Adafruit_GPS* GPS;
        HardwareSerial* GPSSerial;
    public:
        DigitalGPS(HardwareSerial *ser);
        void initGPS();
        void eraseLOCUS();
        void GPSData_dump_setup();
        void refresh_GPSData(bool GPSECHO);
        void pullGPSFlashData();
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

class FlashOp {
    // Class to manage saving data to and reading data from the flash chip
    struct ourTypes {
        int size = 0; // Size of one sample, in bytes
        int nSamples = 0; // Number of samples stored on chip
        uint32_t start_addr = 0; // Start address of this type's allocated memory
        void* data;
        float f;
    };
    
    struct event {
        uint32_t t;
        char ident;
    };

    private:
        SPIFlash* flash;

        int nTypes = -1;
        int maxTypes = 5;
        ourTypes dataTypes[5];

        int nEvents = -1;
        int maxEvents = 5;
        event events[5];

        bool running = false;

    public:
        // init
        FlashOp(SPIFlash* flash);
        bool begin();
        int addType(int size, float f, void* data);

        // Writing
        bool addSample(int ident);
        bool addEvent(uint32_t t, char ident);

        // Reading
        bool getType(int ident, int* size);
        bool getSample(int ident, int sample, void* data);
};