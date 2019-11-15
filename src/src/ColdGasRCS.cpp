#include "yonics.hpp"
#include <Servo.h>
#include "math.h"

#define SERVO_PIN_0 6
/*#define SOLENOID_PIN_1 36
#define SOLENOID_PIN_2 37
#define SOLENOID_PIN_3 38*/

#define GOAL 0.262//0.523599//.4363 //rad
#define K0 0
#define K1 .5
#define K2 9.8
#define THRESHOLD 0.1
#define ACTUAL_TORQUE 10 
#define INTERVAL 50 //ms

#define MAX_ERROR 15 // radian seconds
#define MIN_ERROR -15

Servo jim;

double error = 0; 
double last_millis;

void printStuff(double current, double omega, double error, double ctrl_torque, bool isOpen, IMUdata tempIMU){
    Serial.printf("%u,%.4f,%.4f,%.4f,%.4f,%u,     ", millis(), current, omega, error, ctrl_torque, isOpen);
    Serial.printf("%.4f,%.4f,%.4f,",tempIMU.orient_euler[0],tempIMU.orient_euler[1],tempIMU.orient_euler[2]);
    Serial.printf("%.4f,%.4f,%.4f,%.4f\n",tempIMU.orient_quat[0],tempIMU.orient_quat[1],tempIMU.orient_quat[2],tempIMU.orient_quat[3]);
}

ColdGasRCS::ColdGasRCS() {
    init();
}

void ColdGasRCS::init() {
    pinMode(SERVO_PIN_0, OUTPUT);
    last_millis = millis();
    jim.attach(SERVO_PIN_0);
    /*pinMode(SOLENOID_PIN_1, OUTPUT);
    pinMode(SOLENOID_PIN_2, OUTPUT);
    pinMode(SOLENOID_PIN_3, OUTPUT);*/
}



void ColdGasRCS::adjust(double current, double omega, SaveSD* saver, IMUdata imu){
    double this_error = GOAL - current;
    error += this_error * ((millis() - last_millis)/1000);
    if(error > MAX_ERROR){
        error = MAX_ERROR;
    } else if ( error < MIN_ERROR) {
        error = MIN_ERROR;
    }
    last_millis = millis();

    double ctrl_torque = K2*(GOAL-current) - K1*abs(omega) + K0*error;
    if (ctrl_torque >= 9){
        ctrl_torque = 9;
    }
    if (ctrl_torque <=0){
        ctrl_torque = 0;
    }
    ctrl_torque = ctrl_torque/7;
    // Serial.print("Control Torque: ");
    // Serial.println(ctrl_torque);
    double angle = polyfitfunct(ctrl_torque);
    jim.write(angle);
    Serial.printf("%d,%d,%d,%d,%d\n",millis(),current,omega,ctrl_torque,angle);
}

int ColdGasRCS::getInterval(){
    return INTERVAL;
}

double ColdGasRCS::polyfitfunct(double x){
        double angle = pow(15264*x,8) - pow(6.5224E4*x,7) + pow(1.1440E5*x,6) - pow(1.0655E5*x,5) + pow(5.6864E4*x,4) - pow(1.7472E4*x,3) + pow(2.9237E3*x,2) - 261.1007*x + 56.1953;
        return angle;
}
