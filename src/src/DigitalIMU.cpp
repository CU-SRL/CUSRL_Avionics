#include "yonics.hpp"

DigitalIMU::DigitalIMU() {
    board = Adafruit_BNO055(55, 0x28);
}

DigitalIMU::DigitalIMU(int32_t sensorID, uint8_t address) {
    board = Adafruit_BNO055(sensorID, address);
}

bool DigitalIMU::begin() {
    return board.begin();
}

void DigitalIMU::sample(IMUdata* data) {
    // Get data and store it to the imu_data struct
    board.getEvent(&event,Adafruit_BNO055::VECTOR_LINEARACCEL);
    data->accelerometer[0] = event.acceleration.x;
    data->accelerometer[1] = event.acceleration.y;
    data->accelerometer[2] = event.acceleration.z;

    board.getEvent(&event,Adafruit_BNO055::VECTOR_GYROSCOPE);
    data->gyroscope[0] = event.gyro.x;
    data->gyroscope[1] = event.gyro.y;
    data->gyroscope[2] = event.gyro.z;

    board.getEvent(&event,Adafruit_BNO055::VECTOR_EULER);
    data->orientation[0] = event.orientation.x;
    data->orientation[1] = event.orientation.y;
    data->orientation[2] = event.orientation.z;
    
    board.getEvent(&event,Adafruit_BNO055::VECTOR_MAGNETOMETER);
    data->magnetometer[0] = event.magnetic.x;
    data->magnetometer[1] = event.magnetic.y;
    data->magnetometer[2] = event.magnetic.z;

    data->t = millis();
}