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
int speakerPin = 2;
int highG_xPin = 33;
int highG_yPin = 34;
int highG_zPin = 35;

// Intervals (ms)
int interval_IMU = 45;
int interval_BAROM = 2000;
int interval_ACCEL = 50;
int interval_GPS = 1/10;
int interval_RF = 2000;

// ========== PROTOTHREADING ===========

// ThreadController that will control all threads
ThreadController thread_control = ThreadController();

// Throw some threads on the heap
Thread* ThreadIMU = new Thread();
Thread* ThreadBAROM = new Thread();
Thread* ThreadACCEL = new Thread();
Thread* ThreadRF = new Thread();
//Thread* ThreadGPS = new Thread();

// ========== SENSORS AND DATA ==========

// Define the GPS hardware serial port
#define GPSSerial Serial3
#define GPSECHO false // False to turn off echoing of GPS Data to Serial

// Initializes Sensor classes
DigitalIMU IMU = DigitalIMU(55,0x28);
DigitalBAROM BAROM;
AnalogIMU HIGHG = AnalogIMU(highG_xPin,highG_yPin,highG_zPin,true);

#define RF_TYPE 0 // SERVER(1) or CLIENT(0)

RFM96W_Server *GROUND_STATION;
RFM96W_Client *CLIENT;

// Variables to store sensor data
GPSdata gps_data;
IMUdata imu_data;
BAROMdata barom_data;
ACCELdata accel_data;

// Piezo beeper!
BeepyBOI berp = BeepyBOI(speakerPin);

// ========== DATA SAVING ==========


SaveSD saver;
DigitalGPS* gps_ptr;

void thread_GPS() {
    //saver.sampleGPS(&gps_data);
    // Refresh the GPS Data
    gps_ptr->refresh_GPSData(GPSECHO);
    gps_ptr->pullRawGPS(/*&gps_data*/);
}

void thread_IMU() {
    // Sample IMU
    IMU.sample(&imu_data);

    saver.sampleIMU(&imu_data);
}

void thread_BAROM() {
    // Sample barometer
    BAROM.sample(&barom_data);

    saver.sampleBAROM(&barom_data);
}

void thread_HIGHG() {
    // Sample high-g accelerometer
    HIGHG.sample(&accel_data);

    saver.sampleACCEL(&accel_data);
}

void thread_RF() {
    float float_data = imu_data.gyro_fused[0];
    // Proxy array of uint8_t values
    uint8_t proxyArr[sizeof(float)];
    // Populate array with zeros
    for (int i=0;i<sizeof(float);i++) {proxyArr[i] = 0;}
    // Copy float data to array
    memcpy(&proxyArr,&float_data,sizeof(float));

    CLIENT->loop(proxyArr);
}

void KILLSYSTEM() {
    while(true) {
        delay(500); // Just to make it do something
        berp.error();
    }
}

void setup() {

    delay(2500);

    // Start serial
    Serial.begin(115200);

    // Hello beep
    berp.hello();

    // Initialize file saving
    saver.initFolder();

    // Initialize BNO055 IMU sensor
    if (!IMU.begin()) {
        KILLSYSTEM();
    }

    // Initialize MPL3115A2 sensor
    /*if (!BAROM.begin()) {
        KILLSYSTEM();
    }*/

    //gps_ptr = new DigitalGPS(&Serial3);

    // Initialize the GPS Data Dump
    //gps_ptr->GPSData_dump_setup();

    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    //gps_ptr->eraseLOCUS();
    //gps_ptr->initGPS();
    // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    if(RF_TYPE==1)
    {
        GROUND_STATION = new RFM96W_Server(16, 14, hardware_spi);
    }
    else if (RF_TYPE==0)
    {
        CLIENT = new RFM96W_Client(16, 14, hardware_spi);
    }

    // Configure IMU thread
    ThreadIMU->onRun(thread_IMU);
    ThreadIMU->setInterval(interval_IMU);

    // Configure Barometer thread
    /*ThreadBAROM->onRun(thread_BAROM);
    ThreadBAROM->setInterval(interval_BAROM);*/

    // Configure Accelerometer thread
    ThreadACCEL->onRun(thread_HIGHG);
    ThreadACCEL->setInterval(interval_ACCEL);

    // Configure GPS thread
    //ThreadGPS->onRun(thread_GPS);
    //ThreadGPS->setInterval(interval_GPS);

    // Configure RF thread
    ThreadRF->onRun(thread_RF);
    ThreadRF->setInterval(interval_RF);

    // Add threads to controller
    thread_control.add(ThreadIMU);
    //thread_control.add(ThreadBAROM);
    thread_control.add(ThreadACCEL);
    thread_control.add(ThreadRF);
    //thread_control.add(ThreadGPS);

    // Beep the piezo again
    berp.bombBeep();
}

void loop() {
    thread_control.run();
}