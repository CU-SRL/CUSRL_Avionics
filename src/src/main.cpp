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
#include "yonics.hpp"


// ========== DEFINE SOME VARS ==========

// Pin assignments
int flashPin = 29;
int speakerPin = 36;
int highG_xPin = 33;
int highG_yPin = 34;
int highG_zPin = 35;

// Intervals
int interval_GPS = 100;
int interval_IMU = 40;
int interval_BAROM = 1500;
int interval_ACCEL = 40;

// ========== PROTOTHREADING ===========

// ThreadController that will control all threads
ThreadController thread_control = ThreadController();

//Threads (as a pointer)
Thread* ThreadGPS = new Thread();
Thread* ThreadIMU = new Thread();
Thread* ThreadBAROM = new Thread();
Thread* ThreadACCEL = new Thread();

// ========== SENSORS AND DATA ==========

// Define the GPS hardware serial port
#define GPSSerial Serial3
// Initialize the GPS on the hardware port
//Adafruit_GPS GPS(&GPSSerial);
#define GPSECHO false // False to turn off echoing of GPS Data to Serial

// Initializes Sensors
// Adafruit_BNO055 IMU = Adafruit_BNO055(55, 0x28);
DigitalIMU IMU = DigitalIMU(55,0x28);
// Adafruit_MPL3115A2 BAROM = Adafruit_MPL3115A2();
DigitalBAROM BAROM;
AnalogIMU HIGHG = AnalogIMU(highG_xPin,highG_yPin,highG_zPin,true);


// Variables to store sensor data
sensors_event_t event; 
GPSdata gps_data;
IMUdata imu_data;
BAROMdata barom_data;
Acceldata accel_data;

// Piezo beeper!
BeepyBOI berp = BeepyBOI(speakerPin);

// ========== FLASH CHIP AND DATA SAVING ==========

// Initialize flash chip
SPIFlash flash(flashPin);

// uint32_t addr_GPS;
// uint32_t addr_IMU;
// uint32_t addr_BAROM;

// uint16_t counter_GPS = 0;
// uint16_t counter_IMU = 0;
// uint16_t counter_BAROM = 0;

uint32_t GPSDataSize;
uint32_t imuDataSize;
uint32_t baromDataSize;
uint32_t accelDataSize;

SaveSD saver;
DigitalGPS* gps_ptr;
FlashOp flashop = FlashOp(&flash);

void thread_GPS()
{
    // Refresh the GPS Data
    gps_ptr->refresh_GPSData(GPSECHO);

    /*gps_data.altitude = GPS.altitude;
    gps_data.angle = GPS.angle;
    gps_data.lat = GPS.latitudeDegrees;
    gps_data.lon = GPS.longitudeDegrees;
    gps_data.sat_num = GPS.satellites;
    gps_data.speed = GPS.speed;*/

    /*Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    }*/

    // Write data struct to flash chip
    /*if (!flash.writeAnything(addr_GPS+=GPSDataSize,gps_data)) {
        // Serial.println("Error writing data to flash.");
    }
    else {
        counter_GPS++;
    }*/
}

void thread_IMU() {

    IMU.sample(&imu_data);

    // Write data struct to flash chip
    flashop.addSample(0);

    // if (!flash.writeAnything(addr_IMU+=imuDataSize,imu_data)) {
    //     // Serial.println("Error writing data to flash.");
    // }
}

void thread_BAROM() {
    BAROM.sample(&barom_data);

    {
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
    }
    
    // Write data struct to flash chip
    flashop.addSample(1);

    // if (!flash.writeAnything(addr_BAROM+=baromDataSize,barom_data)) {
    //     // Serial.println("Error writing data to flash.");
    // }
    // else {
    //     counter_BAROM++;
    // }
}

void thread_HIGHG() {
    HIGHG.sample(&accel_data);

    // Write to flash
    flashop.addSample(2);
}

void KILLSYSTEM() {
    while(true) {
        delay(500); // Just to make it do something
        berp.error();
    }
}

void setup() {
    berp.hello();

    // Start serial
    Serial.begin(115200);

    DigitalGPS gps(&GPSSerial);

    // Initialize BNO055 IMU sensor
    if (!IMU.begin()) {
        // Serial.println("Couldn't find sensor BNO055");
        KILLSYSTEM();
    }

    // Initialize MPL3115A2 sensor
    if (!BAROM.begin()) {
        // Serial.println("Couldn't find sensor MPL3115A2");
        KILLSYSTEM();
    }

    // Sizing of data structs
    GPSDataSize = sizeof(gps_data);
    imuDataSize = sizeof(imu_data);
    baromDataSize = sizeof(barom_data);
    accelDataSize = sizeof(accel_data);

    // Add data types to flashop
    if (!flashop.addType(imuDataSize,interval_IMU,&imu_data)) {KILLSYSTEM();}
    if (!flashop.addType(baromDataSize,interval_BAROM,&barom_data)) {KILLSYSTEM();}
    if (!flashop.addType(accelDataSize,interval_ACCEL,&accel_data)) {KILLSYSTEM();}

    // Initialize flash chip
    if (!flashop.begin()) {KILLSYSTEM();}

    // Initialize the GPS Data Dump
    gps.GPSData_dump_setup();

    // Initialize flash chip
    flash.begin();
    int flashSize = flash.getCapacity();
    saver.addFlash(&flash);

    // Calculate flash chip allocations
    int share_IMU = (float)flashSize*((float)imuDataSize/(float)interval_IMU)/((float)imuDataSize/(float)interval_IMU + (float)baromDataSize/(float)interval_BAROM /*+ (float)GPSDataSize/(float)interval_GPS*/);

    // Copy data to flash chip
    berp.lowBeep();
    berp.hiBeep();
    // Serial.println("Saving to SD card...");
    if (!saver.savenow(imuDataSize,baromDataSize)) {
        KILLSYSTEM();
    }
    berp.lowBeep();
    berp.hiBeep();
    berp.midBeep();
    // Serial.println("Saving complete.");

    berp.countdown(20);

    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    flash.eraseChip();
    gps.eraseLOCUS();
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

    gps.initGPS();

    //  Give the ptr the address of the GPS Object that was created
    gps_ptr=&gps;

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

    // Configure GPS thread
    ThreadGPS->onRun(thread_GPS);
    ThreadGPS->setInterval(interval_GPS);

    // Configure IMU thread
    ThreadIMU->onRun(thread_IMU);
    ThreadIMU->setInterval(interval_IMU);

    // Configure Barometer thread
    ThreadBAROM->onRun(thread_BAROM);
    ThreadBAROM->setInterval(interval_BAROM);

    // Configure Accelerometer thread
    

    // Add threads to controller
    thread_control.add(ThreadIMU);
    thread_control.add(ThreadGPS);
    thread_control.add(ThreadBAROM);

    // Beep the piezo again
    berp.bombBeep();
}

void loop() {
    thread_control.run();
}