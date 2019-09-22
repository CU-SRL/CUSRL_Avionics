
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
#include <SdFat.h>

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
        SaveSD() {
            if (!sd.begin()) {
                Serial.println("Error: failed to initialize SD.");
            }
        }
        bool savenow(SPIFlash* flash, uint32_t imuDataSize, uint32_t baromDataSize) {
            File of;
            of = sd.open("datalog.dat", FILE_WRITE);
            of.println("# ====== DATA START ======");
            of.println("# IMU: orient x,y,z gyro x,y,z accel x,y,z megneto x,y,z t[s]");

            uint32_t read_addr_BAROM;
            if (!flash->readAnything(0,read_addr_BAROM)) {
                Serial.println("No start address found. Aborting save.");
                return false;
            }

            IMUdata tempIMU;
            uint32_t read_addr_IMU = 4;

            Serial.print("Saving IMU data...");
            // Save IMU data
            while(!flash->writeByte(read_addr_IMU,69)) {
                flash->readAnything(read_addr_IMU+=imuDataSize,tempIMU);
                of.print(tempIMU.orientation[0]);
                of.print(",");
                of.print(tempIMU.orientation[1]);
                of.print(",");
                of.print(tempIMU.orientation[2]);
                of.print(",");
                of.print(tempIMU.gyroscope[0]);
                of.print(",");
                of.print(tempIMU.gyroscope[1]);
                of.print(",");
                of.print(tempIMU.gyroscope[2]);
                of.print(",");
                of.print(tempIMU.accelerometer[0]);
                of.print(",");
                of.print(tempIMU.accelerometer[1]);
                of.print(",");
                of.print(tempIMU.accelerometer[2]);
                of.print(",");
                of.print(tempIMU.accelerometer[2]);
                of.print(",");
                of.print(tempIMU.magnetometer[0]);
                of.print(",");
                of.print(tempIMU.magnetometer[1]);
                of.print(",");
                of.print(tempIMU.magnetometer[2]);
                of.print(",");
                of.println(tempIMU.t);

                Serial.println("IMU sample saved.");
            }
            Serial.println("IMU data saved.");

            BAROMdata tempBAROM;
            of.println("# Barometer: Altitude,Pressure,Temperature");

            Serial.print("Barom start address: ");
            Serial.println(read_addr_BAROM);

            Serial.print("Saving barom data...");
            // Save barometer data
            while(!flash->writeByte(read_addr_BAROM,69)) {
                flash->readAnything(read_addr_BAROM+=baromDataSize,tempBAROM);
                of.print(tempBAROM.altitude);
                of.print(",");
                of.print(tempBAROM.pressure);
                of.print(",");
                of.println(tempBAROM.temperature);

                Serial.println("Barom sample saved.");
            }
            Serial.println("Barom data saved.");

            of.close();
            return true;
        }
};