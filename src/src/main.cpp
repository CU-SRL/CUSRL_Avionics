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
int flashWP = 29;
int flashPin = 10;
int speakerPin = 2;
int highG_xPin = 33;
int highG_yPin = 34;
int highG_zPin = 35;

// Intervals (ms)
int interval_IMU = 40;
int interval_BAROM = 2000;
int interval_ACCEL = 40;
int interval_GPS = 10000;

// Sampling frequency
float freq_IMU = 1/(float)interval_IMU;
float freq_BAROM = 1/(float)interval_BAROM;
float freq_ACCEL = 1/(float)interval_ACCEL;
float freq_GPS = 1/(float)interval_GPS;

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
SPIFlash flash(flashPin);

uint32_t gpsDataSize;
uint32_t imuDataSize;
uint32_t baromDataSize;
uint32_t accelDataSize;

SaveSD saver;

// Start addresses
uint32_t addr_start_IMU = 0;
uint32_t addr_start_BAROM = 0;
uint32_t addr_start_ACCEL = 0;
uint32_t addr_start_GPS = 0;

// Current addresses
uint32_t addr_curr_IMU = 0;
uint32_t addr_curr_BAROM = 0;
uint32_t addr_curr_ACCEL = 0;
uint32_t addr_curr_GPS = 0;


DigitalGPS* gps_ptr;


void KILLSYSTEM() {
    while(true) {
        delay(500); // Just to make it do something
        berp.error();
    }
}

void thread_GPS() {
    // Refresh the GPS Data
    gps_ptr->refresh_GPSData(GPSECHO);
    gps_ptr->pullRawGPS();

    if (!flash.writeAnything(addr_curr_GPS+=gpsDataSize,gps_data)) {KILLSYSTEM();}
}

void thread_IMU() {
    // Sample IMU
    IMU.sample(&imu_data);

    // Write sample to flash chip
    if (!flash.writeAnything(addr_curr_IMU+=imuDataSize,imu_data)) {KILLSYSTEM();}
}

void thread_BAROM() {
    // Sample barometer
    BAROM.sample(&barom_data);
    
    // Write data struct to flash chip
    if (!flash.writeAnything(addr_curr_BAROM+=baromDataSize,barom_data)) {KILLSYSTEM();}
}

void thread_HIGHG() {

    // Sample high-g accelerometer
    HIGHG.sample(&accel_data);

    // Write to flash
    if (!flash.writeAnything(addr_curr_ACCEL+=accelDataSize,accel_data)) {KILLSYSTEM();}
}

void setup() {

    delay(2500);

    berp.hello();

    // Start serial
    Serial.begin(115200);

    // Hello beep
    berp.hello();

    Serial.println("1");

    // ========== Save Data ======================================

    flash.begin();
    saver.addFlash(&flash);
    digitalWrite(flashWP,HIGH);

    Serial.println("2");

    // Sizing of data structs
    imuDataSize = sizeof(imu_data);
    baromDataSize = sizeof(barom_data);
    accelDataSize = sizeof(accel_data);
    gpsDataSize = sizeof(gps_data);

    // // Set first 16 bytes to 69 bc lol
    // flash.eraseChip();
    // for (int i=0;i<16;i++){flash.writeByte(i,1);}

    // ===========================================================
    // Initialize flash chip components

    uint32_t chipSize,startOffset;

    // Start offset: address offset because first few store other info
    startOffset = 16;

    // Get size of flash chip
    chipSize = flash.getCapacity() - startOffset;

    // Bytes per second for each data type
    float rate_IMU = (float)imuDataSize*freq_IMU;
    float rate_BAROM = (float)baromDataSize*freq_BAROM;
    float rate_ACCEL = (float)accelDataSize*freq_ACCEL;
    float rate_GPS = (float)gpsDataSize*freq_GPS;
    float rate_total = rate_IMU + rate_BAROM + rate_ACCEL + rate_GPS;

    // Print rates to Serial
    Serial.printf("IMU   rate: %.3f\n",rate_IMU);
    Serial.printf("BAROM rate: %.3f\n",rate_BAROM);
    Serial.printf("ACCEL rate: %.3f\n",rate_ACCEL);
    Serial.printf("GPS   rate: %.3f\n",rate_GPS);

    // Fractional share of each data type
    float share_IMU = rate_IMU/rate_total;
    float share_BAROM = rate_BAROM/rate_total;
    float share_ACCEL = rate_ACCEL/rate_total;
    float share_GPS = rate_GPS/rate_total;

    // Print shares to Serial
    Serial.printf("IMU   share: %.3f\n",share_IMU);
    Serial.printf("BAROM share: %.3f\n",share_BAROM);
    Serial.printf("ACCEL share: %.3f\n",share_ACCEL);
    Serial.printf("GPS   share: %.3f\n",share_GPS);

    // Starting address of each type
    addr_start_IMU = startOffset;
    addr_start_BAROM = startOffset + (uint32_t)(share_IMU*chipSize);
    addr_start_ACCEL = addr_start_BAROM + (uint32_t)(share_BAROM*chipSize);
    addr_start_GPS = addr_start_ACCEL + (uint32_t)(share_ACCEL*chipSize);

    // Write starting addresses to flash chip
    flash.writeULong(0,addr_start_IMU);
    flash.writeULong(4,addr_start_BAROM);
    flash.writeULong(8,addr_start_ACCEL);
    flash.writeULong(12,addr_start_GPS);

    // Set current addresses to starting addresses
    addr_curr_IMU = addr_start_IMU;
    addr_curr_BAROM = addr_start_BAROM;
    addr_curr_ACCEL = addr_start_ACCEL;
    addr_curr_GPS = addr_start_GPS;

    // Print starting addresses to Serial
    Serial.print("IMU   start address: ");
    Serial.println(addr_start_IMU);
    Serial.print("BAROM start address: ");
    Serial.println(addr_start_BAROM);
    Serial.print("ACCEL start address: ");
    Serial.println(addr_start_ACCEL);
    Serial.print("GPS   start address: ");
    Serial.println(addr_start_GPS);


    Serial.println("3");

    // ===========================================================
    // Copy data to flash chip
    berp.lowBeep();
    berp.hiBeep();

    if (!saver.savenow()) {KILLSYSTEM();}
    berp.lowBeep();
    berp.hiBeep();
    berp.midBeep();

    berp.countdown(5);
    Serial.println("4");

    flash.eraseChip();

    // ===========================================================

    gps_ptr = new DigitalGPS(&Serial3);
    Serial.println("5");

    // Initialize BNO055 IMU sensor
    if (!IMU.begin()) {
        KILLSYSTEM();
    }

    // Initialize MPL3115A2 sensor
    if (!BAROM.begin()) {
        KILLSYSTEM();
    }

    // Initialize the GPS Data Dump
    gps_ptr->GPSData_dump_setup();

    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    gps_ptr->eraseLOCUS();
    gps_ptr->initGPS();
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

    // Configure IMU thread
    ThreadIMU->onRun(thread_IMU);
    ThreadIMU->setInterval(interval_IMU);

    // Configure Barometer thread
    ThreadBAROM->onRun(thread_BAROM);
    ThreadBAROM->setInterval(interval_BAROM);

    // Configure Accelerometer thread
    ThreadACCEL->onRun(thread_HIGHG);
    ThreadACCEL->setInterval(interval_ACCEL);

    // Configure GPS thread
    ThreadGPS->onRun(thread_GPS);
    ThreadGPS->setInterval(interval_GPS);

    // Add threads to controller
    thread_control.add(ThreadIMU);
    thread_control.add(ThreadBAROM);
    thread_control.add(ThreadACCEL);
    thread_control.add(ThreadGPS);

    // Beep the piezo again
    berp.bombBeep();
}

void loop() {
    thread_control.run();
}