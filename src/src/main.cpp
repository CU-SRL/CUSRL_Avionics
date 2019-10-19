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
int flashWP = 10;
int flashPin = 29;
int speakerPin = 36;
int highG_xPin = 33;
int highG_yPin = 34;
int highG_zPin = 35;

// Intervals (ms)
int interval_GPS = 100;
int interval_IMU = 40;
int interval_BAROM = 1500;
int interval_ACCEL = 40;

// ========== PROTOTHREADING ===========

// ThreadController that will control all threads
ThreadController thread_control = ThreadController();

// Throw some threads on the heap
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
GPSdata gps_data;
IMUdata imu_data;
BAROMdata barom_data;
Acceldata accel_data;

// Piezo beeper!
BeepyBOI berp = BeepyBOI(speakerPin);

// ========== FLASH CHIP AND DATA SAVING ==========

// Initialize flash chip
SPIFlash flashChip(flashPin);
FlashOp flashop = FlashOp(&flashChip);

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

void thread_GPS()
{
    // Refresh the GPS Data
    gps_ptr->refresh_GPSData(GPSECHO);
    gps_ptr->pullRawGPS(&gps_data);

    // Write GPS data to flash
    flashop.addSample(3);

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

    // Serial.println("IMU Sampling.");

    IMU.sample(&imu_data);

    // Write data struct to flash chip
    flashop.addSample(0);

    // if (!flash.writeAnything(addr_IMU+=imuDataSize,imu_data)) {
    //     // Serial.println("Error writing data to flash.");
    // }
}

void thread_BAROM() {

    // Sample barometer
    BAROM.sample(&barom_data);
    
    // Write data struct to flash chip
    flashop.addSample(1);

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

    // if (!flash.writeAnything(addr_BAROM+=baromDataSize,barom_data)) {
    //     // Serial.println("Error writing data to flash.");
    // }
    // else {
    //     counter_BAROM++;
    // }
}

void thread_HIGHG() {

    // Sample high-g accelerometer
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
    // Start serial
    Serial.begin(115200);

    berp.hello();

    // ========== Save Data ======================================

    saver.addFlashOp(&flashop);

    // Copy data to flash chip
    berp.lowBeep();
    berp.hiBeep();
    
    flashop.addWP(flashWP);
    flashop.beginRead();
    
    if (!saver.savenow()) {KILLSYSTEM();}
    berp.lowBeep();
    berp.hiBeep();
    berp.midBeep();

    berp.countdown(5);

    flashop.stopReading(); // THIS FUNCTION ERASES THE FLASH CHIP

    // ===========================================================

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
    if (-1==flashop.addType(imuDataSize,interval_IMU,&imu_data)) {KILLSYSTEM();}
    if (-1==flashop.addType(baromDataSize,interval_BAROM,&barom_data)) {KILLSYSTEM();}
    if (-1==flashop.addType(accelDataSize,interval_ACCEL,&accel_data)) {KILLSYSTEM();}
    if (-1==flashop.addType(accelDataSize,interval_GPS,&gps_data)) {KILLSYSTEM();}

    // Initialize flash chip
    if (!flashop.beginWrite()) {KILLSYSTEM();}

    // Initialize the GPS Data Dump
    // gps.GPSData_dump_setup();

    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    // gps.eraseLOCUS();
    // gps.initGPS();
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

    //  Give the ptr the address of the GPS Object that was created
    gps_ptr=&gps;

    // Configure GPS thread
    // ThreadGPS->onRun(thread_GPS);
    // ThreadGPS->setInterval(interval_GPS);

    // Configure IMU thread
    ThreadIMU->onRun(thread_IMU);
    ThreadIMU->setInterval(interval_IMU);

    // Configure Barometer thread
    ThreadBAROM->onRun(thread_BAROM);
    ThreadBAROM->setInterval(interval_BAROM);

    // Configure Accelerometer thread
    ThreadACCEL->onRun(thread_HIGHG);
    ThreadACCEL->setInterval(interval_ACCEL);

    // Add threads to controller
    thread_control.add(ThreadIMU);
    // thread_control.add(ThreadGPS);
    thread_control.add(ThreadBAROM);
    thread_control.add(ThreadACCEL);

    // Beep the piezo again
    berp.bombBeep();
}

void loop() {
    thread_control.run();
}