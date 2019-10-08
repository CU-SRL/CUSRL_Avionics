
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
    double orientation[3] = {0,0,0};
    double gyroscope[3] = {0,0,0};
    double accelerometer[3] = {0,0,0};
    double magnetometer[3] = {0,0,0};
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
    public:
        SaveSD();
        bool savenow(SPIFlash* flash, uint32_t imuDataSize, uint32_t baromDataSize);
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
        bool begin();
        void sample(Acceldata* data);
};

class DigitalIMU {
    private:
        Adafruit_BNO055 board;
        sensors_event_t event;
    public:
        DigitalIMU();
        DigitalIMU(int32_t sensorID, uint8_t address);
        bool begin();
        void sample(IMUdata* data);
};

class DigitalBAROM {
    private:
        Adafruit_MPL3115A2 BAROM = Adafruit_MPL3115A2();
};

class DigitalGPS{
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

// class FlashOp {
//     // Class to manage saving data to and reading data from the flash chip
// };