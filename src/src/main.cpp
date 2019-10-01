/***************************************************************/
/*                                                             */
/*                                                             */
/*                    CU-SRL AVIONICS TEAM                     */
/*                    MAIN FLIGHT COMPUTER                     */
/*                                                             */
/*                       Jason Popich                          */
/*                       Lyon Foster                           */
/*                       Carter Mak                            */
/*                       Giselle Koo                           */
/*                                                             */
/***************************************************************/

#include "dataStructs.hpp"

// ========== DEFINE SOME VARS ==========

// Pin assignments
int speakerPin = 2;
int flashPin = 29;

// Intervals
int interval_IMU = 40;
int interval_BAROM = 1500;

// ========== PROTOTHREADING ===========

// ThreadController that will controll all threads
ThreadController thread_control = ThreadController();

//Threads (as a pointer)
Thread* ThreadIMU = new Thread();
Thread* ThreadBAROM = new Thread();

// ========== SENSORS AND DATA ==========

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
// Initializes IMU and barometer
Adafruit_BNO055 IMU = Adafruit_BNO055(55, 0x28);
Adafruit_MPL3115A2 BAROM = Adafruit_MPL3115A2();

// Variables to store sensor data
sensors_event_t event; 
IMUdata imu_data;
BAROMdata barom_data;

// ========== FLASH CHIP AND DATA SAVING ==========

// Initialize flash chip
SPIFlash flash(flashPin);

uint32_t addr_IMU;
uint32_t addr_BAROM;

uint16_t counter_IMU = 0;
uint16_t counter_BAROM = 0;

uint32_t imuDataSize;
uint32_t baromDataSize;

SaveSD saver;

void thread_IMU() {
    // Get data and store it to the imu_data struct
    IMU.getEvent(&event,Adafruit_BNO055::VECTOR_LINEARACCEL);
    imu_data.accelerometer[0] = event.acceleration.x;
    imu_data.accelerometer[1] = event.acceleration.y;
    imu_data.accelerometer[2] = event.acceleration.z;

    IMU.getEvent(&event,Adafruit_BNO055::VECTOR_GYROSCOPE);
    imu_data.gyroscope[0] = event.gyro.x;
    imu_data.gyroscope[1] = event.gyro.y;
    imu_data.gyroscope[2] = event.gyro.z;

    IMU.getEvent(&event,Adafruit_BNO055::VECTOR_EULER);
    imu_data.orientation[0] = event.orientation.x;
    imu_data.orientation[1] = event.orientation.y;
    imu_data.orientation[2] = event.orientation.z;
    
    IMU.getEvent(&event,Adafruit_BNO055::VECTOR_MAGNETOMETER);
    imu_data.magnetometer[0] = event.magnetic.x;
    imu_data.magnetometer[1] = event.magnetic.y;
    imu_data.magnetometer[2] = event.magnetic.z;

    imu_data.t = millis();

    // Print data to console (REMOVE)
    // Serial.print("IMU Data: ");
    // Serial.print(" x: ");
    // Serial.print(imu_data.accelerometer[0]);
    // Serial.print(" | ");
    // Serial.print("IMU Data: ");
    // Serial.print(" y: ");
    // Serial.print(imu_data.accelerometer[1]);
    // Serial.print(" | ");
    // Serial.print("IMU Data: ");
    // Serial.print(" z: ");
    // Serial.print(imu_data.accelerometer[2]);
    // Serial.println("");

    // Write data struct to flash chip
    if (!flash.writeAnything(addr_IMU+=imuDataSize,imu_data)) {
        // Serial.println("Error writing data to flash.");
    }
    else {
        counter_IMU++;
    }

    // tone(speakerPin,6969,10);
}

void thread_BAROM() {
    // Write data to struct
    unsigned long t[4];
    // t[0] = millis();
    barom_data.altitude = BAROM.getAltitude();
    // t[1] = millis();
    barom_data.pressure = BAROM.getPressure();
    // t[2] = millis();
    barom_data.temperature = BAROM.getTemperature();
    // t[3] = millis();
    barom_data.t = millis();

    // PRINTING
    // Serial.println("Timings: ");
    // Serial.println(t[1]-t[0]);
    // Serial.println(t[2]-t[1]);
    // Serial.println(t[3]-t[2]);

    // Serial.print("Barometer data: ");
    // Serial.print("Altitude: ");
    // Serial.print(barom_data.altitude);
    // Serial.print(" | Pressure: ");
    // Serial.print(barom_data.pressure);
    // Serial.print(" | Temperature: ");
    // Serial.print(barom_data.temperature);
    // Serial.println("");

    // Write data struct to flash chip
    if (!flash.writeAnything(addr_BAROM+=baromDataSize,barom_data)) {
        // Serial.println("Error writing data to flash.");
    }
    else {
        counter_BAROM++;
    }
}

void setup() {
    // Beep piezo
    tone(speakerPin,440,200); // hehe concert A
    delay(1000);

    // Start serial
    Serial.begin(115200);

    // Initialize BNO055 IMU sensor
    if (!IMU.begin()) {
        // Serial.println("Couldn't find sensor BNO055");
        return;
    }

    // Initialize MPL3115A2 sensor
    if (!BAROM.begin()) {
        // Serial.println("Couldn't find sensor MPL3115A2");
        return;
    }

    // Sizing of data structs
    imuDataSize = sizeof(imu_data);
    baromDataSize = sizeof(barom_data);

    // Initialize flash chip
    flash.begin();
    int flashSize = flash.getCapacity();

    // Calculate flash chip allocations
    int share_IMU = (float)flashSize*((float)imuDataSize/(float)interval_IMU)/((float)imuDataSize/(float)interval_IMU + (float)baromDataSize/(float)interval_BAROM);

    // Copy data to flash chip
    tone(speakerPin,5000,1000);
    delay(1000);
    // Serial.println("Saving to SD card...");
    if (!saver.savenow(&flash,imuDataSize,baromDataSize)) {
        tone(speakerPin,300,4000);
        delay(4500);
    }
    delay(250);
    tone(speakerPin,5000,1000);
    delay(1000);
    // Serial.println("Saving complete.");

    for(int i=0;i<20;i++) {
        tone(speakerPin,500,20);
        delay(1000);
    }

    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    flash.eraseChip(); // DELETE THIS MOTHERFUCKER
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

    // Serial.print("flashSize: ");
    // Serial.println(flashSize);
    // Serial.print("IMU size  : ");
    // Serial.println(share_IMU);
    // Serial.print("Barom size: ");
    // Serial.println(flashSize-share_IMU);

    // Define starting addresses
    addr_IMU = 4;
    addr_BAROM = share_IMU+5;

    // Store starting address of barom data
    flash.writeAnything(0,addr_BAROM);

    // Configure IMU thread
    ThreadIMU->onRun(thread_IMU);
    ThreadIMU->setInterval(interval_IMU);

    // Configure Barometer thread
    ThreadBAROM->onRun(thread_BAROM);
    ThreadBAROM->setInterval(interval_BAROM);

    // Add threads to controller
    thread_control.add(ThreadIMU);
    thread_control.add(ThreadBAROM);

    // Beep the piezo again
    for(int i=0;i<10;i++) {
        tone(speakerPin,1000,25);
        delay(50);
    }
    delay(250);
    tone(speakerPin,3000,250);
    delay(250);
}

void loop() {
    thread_control.run();
}