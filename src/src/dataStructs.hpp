
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "Thread.h"
#include <ThreadController.h>
#include <utility/imumaths.h>
#include <SPIMemory.h>

struct IMUdata {
    double orientation[3] = {0,0,0};
    double gyroscope[3] = {0,0,0};
    double accelerometer[3] = {0,0,0};
    double magnetometer[3] = {0,0,0};
    unsigned long t = 0;
};

struct BAROMdata {
    float pressure = 0;
    float altitude = 0;
    float temperature = 0;
    unsigned long t = 0;
};
