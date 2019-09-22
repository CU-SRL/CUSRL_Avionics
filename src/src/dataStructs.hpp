#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "Thread.h"
#include <ThreadController.h>
#include <utility/imumaths.h>

struct IMUdata {
    double orientation[3];
    double gyroscope[3];
    double accelerometer[3];
    double magnetometer[3];
};

struct BAROMdata {
    float pressure;
    float altitude;
    float temperature;
};
