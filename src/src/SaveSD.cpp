#include "yonics.hpp"

SaveSD::SaveSD() {
    if (!sd.begin()) {
        Serial.println("Error: failed to initialize SD.");
    }
}

bool SaveSD::savenow(uint32_t imuDataSize, uint32_t baromDataSize) {
    
    uint32_t read_addr_BAROM;
    if (!flash->readAnything(0,read_addr_BAROM)) {
        Serial.println("No start address found. Aborting save.");
        return false;
    }

    if (!openFile()) {return false;}

    printIMU(imuDataSize);

    printBAROM(baromDataSize, read_addr_BAROM);

    of.close();
    return true;
}

void SaveSD::printIMU(uint32_t imuDataSize) {
    of.println("# IMU:");
    of.println("orient_euler_x,orient_euler_y,orient_euler_z,gyro_fused_x,gyro_fused_y,gyro_fused_z,accel_fused_x,accel_fused_y,accel_fused_z,accel_raw_x,accel_raw_y,accel_raw_z,gyro_raw_x,gyro_raw_y,gyro_raw_z,magnetometer_x,magnetometer_y,magnetometer_z,orient_quat_w,orient_quat_x,orient_quat_y,orient_quat_z,");

    IMUdata tempIMU;
    uint32_t read_addr_IMU = 4;

    // Serial.print("Saving IMU data...");
    // Save IMU data
    while(!flash->writeByte(read_addr_IMU,69)) {
        flash->readAnything(read_addr_IMU+=imuDataSize,tempIMU);

        of.printf("%.4f,%.4f,%.4f",tempIMU.orient_euler[0],tempIMU.orient_euler[1],tempIMU.orient_euler[2]);
        of.print(",");

        of.printf("%.4f,%.4f,%.4f",tempIMU.gyro_fused[0],tempIMU.gyro_fused[1],tempIMU.gyro_fused[2]);
        of.print(",");
        
        of.printf("%.4f,%.4f,%.4f",tempIMU.accel_fused[0],tempIMU.accel_fused[1],tempIMU.accel_fused[2]);
        of.print(",");

        of.printf("%.4f,%.4f,%.4f",tempIMU.accel_raw[0],tempIMU.accel_raw[1],tempIMU.accel_raw[2]);
        of.print(",");

        of.printf("%.4f,%.4f,%.4f",tempIMU.gyro_raw[0],tempIMU.gyro_raw[1],tempIMU.gyro_raw[2]);
        of.print(",");

        of.printf("%.4f,%.4f,%.4f",tempIMU.magnetometer[0],tempIMU.magnetometer[1],tempIMU.magnetometer[2]);
        of.print(",");

        of.printf("%.4f,%.4f,%.4f,%.4f",tempIMU.orient_quat[0],tempIMU.orient_quat[1],tempIMU.orient_quat[2],tempIMU.orient_quat[3]);
        of.print(",");

        of.printf("%u",tempIMU.t);
        of.println("");

        // Serial.println("IMU sample saved.");
    }
    // Serial.println("IMU data saved.");
}

void SaveSD::printBAROM(uint32_t baromDataSize, uint32_t read_addr_BAROM) {
    BAROMdata tempBAROM;
    of.println("# Barometer: Altitude,Pressure,Temperature");

    // Serial.print("Barom start address: ");
    // Serial.println(read_addr_BAROM);

    Serial.print("Saving barom data...");
    // Save barometer data
    while(!flash->writeByte(read_addr_BAROM,69)) {
        flash->readAnything(read_addr_BAROM+=baromDataSize,tempBAROM);
        of.print(tempBAROM.altitude);
        of.print(",");
        of.print(tempBAROM.pressure);
        of.print(",");
        of.println(tempBAROM.temperature);

        // Serial.println("Barom sample saved.");
    }
    // Serial.println("Barom data saved.");
}

bool SaveSD::openFile() {
    uint16_t counter = 1;
    int charLen = 14;
    char filename [charLen];

    while(true) {
        sprintf(filename,"datalog%03d.csv",counter);

        if(sd.exists(filename)) {
            counter+=1;
            if(counter>999){return false;}
        }
        else {
            if (sd.open(filename, FILE_WRITE)) {return true;}
            else {return false;}
        }
    }
}

bool SaveSD::addFlash(SPIFlash* flash) {
    if (this->flash==NULL) {
        this->flash = flash;
        return true;
    }
    else {
        return false;
    }
}