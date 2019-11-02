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
int speakerPin = 2;
int highG_xPin = 33;
int highG_yPin = 34;
int highG_zPin = 35;

// Intervals (ms)
int interval_IMU = 40;
int interval_BAROM = 1500;
int interval_ACCEL = 40;
int interval_GPS = 100;

// ========== PROTOTHREADING ===========

// ThreadController that will control all threads
ThreadController thread_control = ThreadController();

// Throw some threads on the heap
Thread* ThreadIMU = new Thread();
Thread* ThreadBAROM = new Thread();
Thread* ThreadACCEL = new Thread();
Thread* ThreadGPS = new Thread();

// ========== SENSORS AND DATA ==========

// Define the GPS hardware serial port
#define GPSSerial Serial3
#define GPSECHO false // False to turn off echoing of GPS Data to Serial

// Initializes Sensor classes
DigitalIMU IMU = DigitalIMU(55,0x28);
DigitalBAROM BAROM;
AnalogIMU HIGHG = AnalogIMU(highG_xPin,highG_yPin,highG_zPin,true);


// Variables to store sensor data
GPSdata gps_data;
IMUdata imu_data;
BAROMdata barom_data;
ACCELdata accel_data;

// Piezo beeper!
BeepyBOI berp = BeepyBOI(speakerPin);

// ========== FLASH CHIP AND DATA SAVING ==========

// Initialize flash chip
SPIFlash flashChip(flashPin);
FlashOp flashop = FlashOp(&flashChip);

uint32_t GPSDataSize;
uint32_t imuDataSize;
uint32_t baromDataSize;
uint32_t accelDataSize;

SaveSD saver;
//DigitalGPS* gps_ptr;

/*void thread_GPS()
{
    // Refresh the GPS Data
    gps_ptr->refresh_GPSData(GPSECHO);
    gps_ptr->pullRawGPS(&gps_data);
}*/

void thread_IMU() {
    // Sample IMU
    IMU.sample(&imu_data);

    // Write sample to flash chip
    flashop.writeIMU(&imu_data);
}

void thread_BAROM() {
    // Sample barometer
    BAROM.sample(&barom_data);
    
    // Write data struct to flash chip
    flashop.writeBAROM(&barom_data);
}

void thread_HIGHG() {

    // Sample high-g accelerometer
    HIGHG.sample(&accel_data);

    // Write to flash
    flashop.writeACCEL(&accel_data);
}

void KILLSYSTEM() {
    while(true) {
        delay(500); // Just to make it do something
        berp.error();
    }
}

void setup() {

    delay(5000);

    berp.hello();

    // Start serial
    Serial.begin(115200);

    // Hello beep
    berp.hello();
    Serial.println("1");

    // ========== Save Data ======================================

    saver.addFlashOp(&flashop);
    flashop.addWP(flashWP);

    Serial.println("2");

    // Sizing of data structs
    imuDataSize = sizeof(imu_data);
    baromDataSize = sizeof(barom_data);
    accelDataSize = sizeof(accel_data);
    GPSDataSize = sizeof(gps_data);

    // Initialize flash chip components
    flashop.setIMU(imuDataSize,1/((float)interval_IMU));
    flashop.setBAROM(baromDataSize,1/((float)interval_BAROM));
    flashop.setACCEL(accelDataSize,1/((float)interval_ACCEL));

    Serial.println("3");

    // Copy data to flash chip
    berp.lowBeep();
    berp.hiBeep();
    delay(500);
    
    if (!saver.savenow()) {KILLSYSTEM();}
    berp.lowBeep();
    berp.hiBeep();
    berp.midBeep();

    berp.countdown(5);

    flashop.startWriting(); // ERASES FLASH CHIP

    // ===========================================================

    //DigitalGPS gps(&GPSSerial);

    // Initialize BNO055 IMU sensor
    if (!IMU.begin()) {
        KILLSYSTEM();
    }

    // Initialize MPL3115A2 sensor
    if (!BAROM.begin()) {
        KILLSYSTEM();
    }

    // Initialize the GPS Data Dump
    //gps.GPSData_dump_setup();

    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    //gps.eraseLOCUS();
    //gps.initGPS();
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

    //  Give the ptr the address of the GPS Object that was created
    //gps_ptr=&gps;

    // Configure GPS thread
    //ThreadGPS->onRun(thread_GPS);
    //ThreadGPS->setInterval(interval_GPS);

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