#include "yonics.hpp"

SaveSD::SaveSD() {

    imuDataSize = sizeof(tempIMU);
    baromDataSize = sizeof(tempBAROM);
    accelDataSize = sizeof(tempACCEL);
    gpsDataSize = sizeof(tempGPS);

    if (sd.begin()) {
        running = true;
    }
}

bool SaveSD::addFlash(SPIFlash* flash) {

    if (this->flash==NULL) {
        this->flash = flash;
        return true;
    }
    else {return false;}
}

bool SaveSD::savenow () {
    if (!running) {return false;}

    uint16_t counter = 1;
    int charLen = 14;
    char filename [charLen];

    while(true) {
        sprintf(filename,"datalog%03d.csv",counter);

        if(sd.exists(filename)) {
            counter+=1;
            if(counter>999){return false;}
        }
        else
        {
            break;
        }
        
    }
    of = sd.open(filename,FILE_WRITE);

    Serial.println("File opened!");
    Serial.printf("imuDataSize: %d\n",imuDataSize);

    printIMU();
    printBAROM();
    printACCEL();
    printGPS();

    of.close();

    Serial.println("File closed!");

    return true;
}

void SaveSD::printIMU() {
    of.println("# IMU:");
    of.println("orient_euler_x,orient_euler_y,orient_euler_z,gyro_fused_x,gyro_fused_y,gyro_fused_z,accel_fused_x,accel_fused_y,accel_fused_z,accel_raw_x,accel_raw_y,accel_raw_z,gyro_raw_x,gyro_raw_y,gyro_raw_z,magnetometer_x,magnetometer_y,magnetometer_z,orient_quat_w,orient_quat_x,orient_quat_y,orient_quat_z,");

    uint32_t addr_read_IMU;

    flash->readAnything(0,addr_read_IMU);

    while(!flash->writeByte(addr_read_IMU,69)) {
        flash->readAnything(addr_read_IMU+=imuDataSize,tempIMU);

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
    }
}

void SaveSD::printBAROM() {
    of.println("# Barometer:");
    of.println("Altitude,Pressure,Temperature");

    uint32_t addr_read_BAROM;

    flash->readAnything(4,addr_read_BAROM);

    while(!flash->writeByte(addr_read_BAROM,69)) {
        flash->readAnything(addr_read_BAROM+=baromDataSize,tempBAROM);

        of.printf("%.4f,%.4f,%.4f\n",tempBAROM.altitude,tempBAROM.pressure,tempBAROM.temperature);
    }
}

void SaveSD::printACCEL() {
    of.println("# Accelerometer:");
    of.println("x,y,z");

    uint32_t addr_read_ACCEL;

    flash->readAnything(8,addr_read_ACCEL);

    while(!flash->writeByte(addr_read_ACCEL,69)) {
        flash->readAnything(addr_read_ACCEL+=accelDataSize,tempACCEL);
        of.printf("%.4f,%.4f,%.4f\n",tempACCEL.x,tempACCEL.y,tempACCEL.z);
    }
}

void SaveSD::printGPS() {
    of.println("# GPS:");
    of.println("lat,lon,altitude,speed,angle,sat_num");

    uint32_t addr_read_GPS;

    flash->readAnything(12,addr_read_GPS);

    while(!flash->writeByte(addr_read_GPS,69)) {
        flash->readAnything(addr_read_GPS+=gpsDataSize,tempGPS);
        of.printf("%.10f,%.10f,%.4f,%.4f,%.10f,%f\n",tempGPS.lat,tempGPS.lon,tempGPS.altitude,tempGPS.speed,tempGPS.angle,tempGPS.sat_num);
    }

}
