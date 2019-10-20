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
int interval_GPS = 1/10;
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
Thread* ThreadColdGasRCS = new Thread();
Thread* ThreadACCEL = new Thread();

// ========== SENSORS AND DATA ==========

// Define the GPS hardware serial port
#define GPSSerial Serial3
#define GPSECHO false // False to turn off echoing of GPS Data to Serial

// Initializes Sensors
DigitalIMU IMU = DigitalIMU(55,0x28);
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
//SPIFlash flashChip(flashPin);
//FlashOp flashop = FlashOp(&flashChip);

uint32_t GPSDataSize;
uint32_t imuDataSize;
uint32_t baromDataSize;
uint32_t accelDataSize;

SaveSD* saver;
DigitalGPS* gps_ptr;

// ========== CONTROL ==========

ColdGasRCS* rcs_ptr;


// ========== THREADS ==========

void thread_ColdGasRCS(){
    //do stuff here. 
    double current = (90 - imu_data.orient_euler[1])/180 * 3.14159;
    Serial.print("RCS Euler: ");
    Serial.println(current);
    double omega = -imu_data.gyro_fused[1]/180*3.14159; // not sure which we want LOL raw or fused
    Serial.print("angular rate: ");
    Serial.println(omega);

    rcs_ptr->adjust(current, omega, saver);


}

void thread_GPS()
{
    // Refresh the GPS Data
    gps_ptr->refresh_GPSData(GPSECHO);
    gps_ptr->pullRawGPS();
}

void thread_IMU() {

    // Serial.println("IMU Sampling.");

    IMU.sample(&imu_data);

    // Write data struct to flash chip
    //flashop.addSample(0);
}

void thread_BAROM() {
    // Sample barometer
    BAROM.sample(&barom_data);
    
    // Write data struct to flash chip
    //flashop.addSample(1);
}

void thread_HIGHG() {

    // Sample high-g accelerometer
    HIGHG.sample(&accel_data);

    // Write to flash
    //flashop.addSample(2);
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

    //saver.addFlashOp(&flashop);
    saver = new SaveSD();

    // Copy data to flash chip
    berp.lowBeep();
    berp.hiBeep();
    
    //flashop.addWP(flashWP);
    //flashop.beginRead();
    
    //if (!saver.savenow()) {KILLSYSTEM();}
    berp.lowBeep();
    berp.hiBeep();
    berp.midBeep();

    berp.countdown(5);

    //flashop.stopReading(); // THIS FUNCTION ERASES THE FLASH CHIP

    // ===========================================================

    // DigitalGPS *GPS = new DigitalGPS(&Serial3);
    //  Give the ptr the address of the GPS Object that was created
    // gps_ptr = GPS;

    rcs_ptr = new ColdGasRCS();
    

    // Initialize BNO055 IMU sensor
    if (!IMU.begin()) {
        Serial.println("Couldn't find sensor BNO055");
        KILLSYSTEM();
    }

    // Initialize MPL3115A2 sensor
    // if (!BAROM.begin()) {
    //     Serial.println("Couldn't find sensor MPL3115A2");
    //    KILLSYSTEM();
    // }

    // Sizing of data structs
    GPSDataSize = sizeof(gps_data);
    imuDataSize = sizeof(imu_data);
    baromDataSize = sizeof(barom_data);
    accelDataSize = sizeof(accel_data);

    // Add data types to flashop
    /*if (-1==flashop.addType(imuDataSize,interval_IMU,&imu_data)) {KILLSYSTEM();}
    if (-1==flashop.addType(baromDataSize,interval_BAROM,&barom_data)) {KILLSYSTEM();}
    if (-1==flashop.addType(accelDataSize,interval_ACCEL,&accel_data)) {KILLSYSTEM();}
    if (-1==flashop.addType(accelDataSize,interval_GPS,&gps_data)) {KILLSYSTEM();}*/

    // Initialize flash chip
    //if (!flashop.beginWrite()) {KILLSYSTEM();}

    // Initialize the GPS Data Dump
    // gps_ptr->GPSData_dump_setup();

    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    // gps_ptr->eraseLOCUS();
    // gps_ptr->initGPS();
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

    // Configure GPS thread
    // ThreadGPS->onRun(thread_GPS);
    // ThreadGPS->setInterval(interval_GPS);

    // Configure IMU thread
    ThreadIMU->onRun(thread_IMU);
    ThreadIMU->setInterval(interval_IMU);

    // Configure Barometer thread
    // ThreadBAROM->onRun(thread_BAROM);
    // ThreadBAROM->setInterval(interval_BAROM);

    // Configure Accelerometer thread
    // ThreadACCEL->onRun(thread_HIGHG);
    // ThreadACCEL->setInterval(interval_ACCEL);

    // Add threads to controller
    thread_control.add(ThreadIMU);
    // thread_control.add(ThreadGPS);
    // thread_control.add(ThreadBAROM);
    // thread_control.add(ThreadACCEL);

    ThreadColdGasRCS->onRun(thread_ColdGasRCS);
    ThreadColdGasRCS->setInterval(rcs_ptr->getInterval());
    thread_control.add(ThreadColdGasRCS);

    // Beep the piezo again
    berp.bombBeep();
}

void loop() {
    thread_control.run();
}