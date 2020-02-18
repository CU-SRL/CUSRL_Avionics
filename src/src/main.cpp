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

void thread_IMU() {
    INITS::IMU.sample(&INITS::imu_data); /*!< Sample the IMU by calling the IMU Sample function */
}

void thread_BAROM() {
    INITS::BAROM.sample(&INITS::barom_data); /*!< Sample barometer by calling the BAROM Sample function */
}

void thread_HIGHG() {
    INITS::HIGHG.sample(&INITS::accel_data); /*!< Sample high-g accelerometer by calling the HIGHG Sample function */
}

void KILLSYSTEM() {
    /*! TO RESET THE SYSTEM POWER MUST BE REMOVED AND THEN REAPPLIED */
    while(true) {
        delay(500);
        INITS::berp.error(); /*! Continous loud obnoxious beeping to alert that the system is kill */
    }
}

void setup() {

    delay(2500); /*!< Wait 2.5 seconds before starting everything up */

    Serial.begin(115200); /*!< Start serial comms */

    INITS::berp.hello(); /*!< Hello beep */

    INITS::flash = new DLLflash(10); /*!< STILL VERY WIP FLASH INITIALIZATION */

    /* Initialize BNO055 IMU sensor */
    if (!INITS::IMU.begin()) {
        KILLSYSTEM();
    }

    /* Initialize MPL3115A2 sensor */
    if (!INITS::BAROM.begin()) {
        KILLSYSTEM();
    }

    /* Configure IMU thread */
    PROTOTHREADING::ThreadIMU->onRun(thread_IMU); /*!< Set the ThreadIMU looping function for the ThreadController */
    PROTOTHREADING::ThreadIMU->setInterval(PROTOTHREADING::interval_IMU); /*!< Set the ThreadIMU refresh rate (Interval) */

    /* Configure Barometer thread */
    PROTOTHREADING::ThreadBAROM->onRun(thread_BAROM); /*!< Set the ThreadBAROM looping function for the ThreadController */
    PROTOTHREADING::ThreadBAROM->setInterval(PROTOTHREADING::interval_BAROM); /*!< Set the ThreadBAROM refresh rate (Interval) */

    /* Configure Accelerometer thread */
    PROTOTHREADING::ThreadACCEL->onRun(thread_HIGHG); /*!< Set the ThreadACCEL looping function for the ThreadController */
    PROTOTHREADING::ThreadACCEL->setInterval(PROTOTHREADING::interval_ACCEL); /*!< Set the ThreadACCEL refresh rate (Interval) */

    /* Add threads to ThreadController */
    PROTOTHREADING::thread_control.add(PROTOTHREADING::ThreadIMU); /*!< Add the ThreadIMU to the ThreadController for continous processing */
    PROTOTHREADING::thread_control.add(PROTOTHREADING::ThreadBAROM); /*!< Add the ThreadBAROM to the ThreadController for continous processing */
    PROTOTHREADING::thread_control.add(PROTOTHREADING::ThreadACCEL); /*!< Add the ThreadACCEL to the ThreadController for continous processing */

    /*! Beep the piezo again */
    INITS::berp.bombBeep();

    // START LOOP AFTER THIS IN VOID() LOOP
}

void loop() {
    /*!< Loop the ThreadController */
    PROTOTHREADING::thread_control.run();
}