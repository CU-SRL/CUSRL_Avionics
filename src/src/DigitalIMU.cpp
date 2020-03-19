#include "yonics.hpp"

DigitalIMU::DigitalIMU() {
    board = Adafruit_BNO055(55, 0x28);
}

DigitalIMU::DigitalIMU(int32_t sensorID, uint8_t address) {
    board = Adafruit_BNO055(sensorID, address);
}

bool DigitalIMU::begin() {
    if (board.begin()) {
        board.enableAutoRange(true); // Hopefully this will enable more than +/-4g???
        return true;
    }
    else {
        return false;
    }
}

void DigitalIMU::sample(IMUdata* data) {
    // Get data and store it to the imu_data struct

    // Processed acceleration data
    board.getEvent(&event,Adafruit_BNO055::VECTOR_LINEARACCEL);
    data->accel_fused[0] = event.acceleration.x;
    data->accel_fused[1] = event.acceleration.y;
    data->accel_fused[2] = event.acceleration.z;

    // Processed gyro data
    board.getEvent(&event,Adafruit_BNO055::VECTOR_GYROSCOPE);
    data->gyro_fused[0] = event.gyro.x;
    data->gyro_fused[1] = event.gyro.y;
    data->gyro_fused[2] = event.gyro.z;
    // Serial.print("Gyro X: ");
    // Serial.print(event.gyro.x);
    // Serial.print(" Gyro Y: ");
    // Serial.print(event.gyro.y);
    // Serial.print(" Gyro Z: ");
    // Serial.println(event.gyro.z);

    // Processed euler orientation vectors
    board.getEvent(&event,Adafruit_BNO055::VECTOR_EULER);
    data->orient_euler[0] = event.orientation.x;
    data->orient_euler[1] = event.orientation.y;
    data->orient_euler[2] = event.orientation.z;

    // 
    quat = board.getQuat();
    data->orient_quat[0] = quat.w();
    data->orient_quat[1] = quat.x();
    data->orient_quat[2] = quat.y();
    data->orient_quat[3] = quat.z();
    
    /*Serial.printf("w acceleration: %.5f",quat.w());
    Serial.printf("x acceleration: %.5f",quat.x());
    Serial.printf("y acceleration: %.5f",quat.y());
    Serial.printf("z acceleration: %.5f",quat.z());*/

    accel = board.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    data->accel_raw[0] = accel.x();
    data->accel_raw[1] = accel.y();
    data->accel_raw[2] = accel.z();

    /*Serial.printf("x acceleration: %.5f",accel.x());
    Serial.printf("y acceleration: %.5f",accel.y());
    Serial.printf("z acceleration: %.5f",accel.z());*/

    board.getEvent(&event,Adafruit_BNO055::VECTOR_MAGNETOMETER);
    data->magnetometer[0] = event.magnetic.x;
    data->magnetometer[1] = event.magnetic.y;
    data->magnetometer[2] = event.magnetic.z;

    /*Serial.printf("x Magnetometer: %.5f",event.magnetic.x);
    Serial.printf("y Magnetometer: %.5f",event.magnetic.y);
    Serial.printf("z Magnetometer: %.5f\n",event.magnetic.z);*/

    data->t = millis();
}