#include "yonics.hpp"

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

SaveSD::SaveSD() {
    if (sd.begin()) {
        running = true;
    }
}

bool SaveSD::initFolder() {
    uint16_t counter = 1;

    while(true) {
        sprintf(foldername,"yonics%04d",counter);

        if(sd.exists(foldername)) {
            counter+=1;
            if(counter>999){return false;}
        }
        else {break;}
    }
    sd.mkdir(foldername);
    sd.chdir(foldername);
    return true;
}

bool SaveSD::openIMU(){
    char filename[7];
    sprintf(filename,"imu.csv");
    of = sd.open(filename,FILE_WRITE);

    return true;
}
bool SaveSD::openBAROM(){
    char filename[9];
    sprintf(filename,"barom.csv");
    of = sd.open(filename,FILE_WRITE);

    return true;
}
bool SaveSD::openACCEL(){
    char filename[9];
    sprintf(filename,"accel.csv");
    of = sd.open(filename,FILE_WRITE);

    return true;
}
bool SaveSD::openGPS(){
    char filename[7];
    sprintf(filename,"gps.csv");
    of = sd.open(filename,FILE_WRITE);

    return true;
}

void SaveSD::printIMU(IMUdata* data) {
    // of.println("# IMU:");
    // of.println("orient_euler_x,orient_euler_y,orient_euler_z,gyro_fused_x,gyro_fused_y,gyro_fused_z,accel_fused_x,accel_fused_y,accel_fused_z,accel_raw_x,accel_raw_y,accel_raw_z,gyro_raw_x,gyro_raw_y,gyro_raw_z,magnetometer_x,magnetometer_y,magnetometer_z,orient_quat_w,orient_quat_x,orient_quat_y,orient_quat_z,");
  
    of.printf("%10f,%10f,%10f",data->orient_euler[0],data->orient_euler[1],data->orient_euler[2]);
    of.print(",");

    of.printf("%10f,%10f,%10f",data->gyro_fused[0],data->gyro_fused[1],data->gyro_fused[2]);
    of.print(",");
    
    of.printf("%10f,%10f,%10f",data->accel_fused[0],data->accel_fused[1],data->accel_fused[2]);
    of.print(",");

    of.printf("%10f,%10f,%10f",data->accel_raw[0],data->accel_raw[1],data->accel_raw[2]);
    of.print(",");

    of.printf("%10f,%10f,%10f",data->gyro_raw[0],data->gyro_raw[1],data->gyro_raw[2]);
    of.print(",");

    of.printf("%10f,%10f,%10f",data->magnetometer[0],data->magnetometer[1],data->magnetometer[2]);
    of.print(",");

    of.printf("%10f,%10f,%10f,%10f",data->orient_quat[0],data->orient_quat[1],data->orient_quat[2],data->orient_quat[3]);
    of.print(",");

    of.printf("%u",data->t);
    of.println("");
}

void SaveSD::printBAROM(BAROMdata* data) {
    // of.println("# Barometer:");
    // of.println("Altitude,Pressure,Temperature");

    of.printf("%10f,%10f,%10f\n",data->altitude,data->pressure,data->temperature);
}

void SaveSD::printACCEL(ACCELdata* data) {
    // of.println("# Accelerometer:");
    // of.println("x,y,z");
    
    of.printf("%10f,%10f,%10f\n",data->x,data->y,data->z);
}

void SaveSD::printGPS(GPSdata* data) {
    // of.println("# GPS:");
    // of.println("lat,lom,altitude,speed,angle,sat_num");

    of.printf("%.10f,%.10f,%10f,%10f,%.10f,%f\n",data->lat,data->lon,data->altitude,data->speed,data->angle,data->sat_num);
}

bool SaveSD::sampleIMU(IMUdata* data) {
    openIMU();
    printIMU(data);
    of.close();
    return true;
}
bool SaveSD::sampleBAROM(BAROMdata* data) {
    openBAROM();
    printBAROM(data);
    of.close();
    return true;
}
bool SaveSD::sampleACCEL(ACCELdata* data) {
    openACCEL();
    printACCEL(data);
    of.close();
    return true;
}
bool SaveSD::sampleGPS(GPSdata* data) {
    openGPS();
    printGPS(data);
    of.close();
    return true;
}
