#include "yonics.hpp"

#define SOLENOID_PIN_0 6
/*#define SOLENOID_PIN_1 36
#define SOLENOID_PIN_2 37
#define SOLENOID_PIN_3 38*/

#define GOAL 0
#define K1 1
#define K2 1
#define THRESHOLD 0
#define ACTUAL_TORQUE 10 
#define INTERVAL 100 //ms


ColdGasRCS::ColdGasRCS() {

    init();
}

void ColdGasRCS::init() {
    pinMode(SOLENOID_PIN_0, OUTPUT);
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

void ColdGasRCS::adjust(double current, double omega){
    double ctrl_torque = K2*(GOAL-current) - K1*omega;
    if(ctrl_torque / ACTUAL_TORQUE > THRESHOLD){
        // set solenoid open
        openSolenoid(0);
    } else {
        //close
        closeSolenoid(0);
    }
}

int ColdGasRCS::getInterval(){
    return INTERVAL;
}