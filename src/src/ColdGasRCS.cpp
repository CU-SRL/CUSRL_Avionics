#include "yonics.hpp"

#define SOLENOID_PIN_0 6
/*#define SOLENOID_PIN_1 36
#define SOLENOID_PIN_2 37
#define SOLENOID_PIN_3 38*/

#define GOAL .4363 //rad
#define K0 .3
#define K1 4.9
#define K2 9.8
#define THRESHOLD 0.1
#define ACTUAL_TORQUE 10 
#define INTERVAL 50 //ms

#define MAX_ERROR 15 // radian seconds
#define MIN_ERROR -15

double error = 0; 
double last_millis;

ColdGasRCS::ColdGasRCS() {
    init();
}

void ColdGasRCS::init() {
    pinMode(SOLENOID_PIN_0, OUTPUT);
    last_millis = millis();
    /*pinMode(SOLENOID_PIN_1, OUTPUT);
    pinMode(SOLENOID_PIN_2, OUTPUT);
    pinMode(SOLENOID_PIN_3, OUTPUT);*/
}

void ColdGasRCS::openSolenoid(int solenoid){
    switch(solenoid){
        case 0:
            digitalWrite(SOLENOID_PIN_0, HIGH);
            break;
        default:
            break;
    }
}

void ColdGasRCS::closeSolenoid(int solenoid){
    switch(solenoid){
        case 0:
            digitalWrite(SOLENOID_PIN_0, LOW);
            break;
        default:
            break;
    }
}

void ColdGasRCS::adjust(double current, double omega, SaveSD* saver){
    double this_error = GOAL - current;
    error += this_error * ((millis() - last_millis)/1000);
    if(error > MAX_ERROR){
        error = MAX_ERROR;
    } else if ( error < MIN_ERROR) {
        error = MIN_ERROR;
    }
    last_millis = millis();

    double ctrl_torque = K2*(GOAL-current) - K1*omega + K0*error;
    Serial.print("Control Torque: ");
    Serial.println(ctrl_torque);
    if(ctrl_torque / ACTUAL_TORQUE > THRESHOLD){
        Serial.println("OPEN");
        // set solenoid open
        openSolenoid(0);
        saver->saveNowRCS(current, omega, error, ctrl_torque, 1);
    } else {
        Serial.println("CLOSE");
        saver->saveNowRCS(current, omega, error, ctrl_torque, 0);    
        //close
        closeSolenoid(0);
    }
}

int ColdGasRCS::getInterval(){
    return INTERVAL;
}
