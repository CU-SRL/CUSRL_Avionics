#include "yonics.hpp"

////////////////////////////////////////////
//Notes
//
// make sure you use the component of the +z axis of the accelerometer
// that is in the direction of flat earth frame z-hat
// so like cos of the acceleration of the twice integrated alpha 
// (theta) from the gyro
//
//theta_z from bno055 fused euler is 2nd index

//Just integration of z-hat acceleration


intAndFilter::intAndFilter(IMUdata* IMU_ptr){
    intAndFilter::IMU_ptr = IMU_ptr;
    
}



double intAndFilter::integrate(){

    // making sure we have all of the stuff
    intAndFilter::a_x = intAndFilter::IMU_ptr->accel_raw[0];
    intAndFilter::a_y = intAndFilter::IMU_ptr->accel_raw[1];
    intAndFilter::a_z = intAndFilter::IMU_ptr->accel_raw[2];

    intAndFilter::alpha_x = intAndFilter::IMU_ptr->gyro_raw[0];
    intAndFilter::alpha_y = intAndFilter::IMU_ptr->gyro_raw[1];
    intAndFilter::alpha_z = intAndFilter::IMU_ptr->gyro_raw[2];

    intAndFilter::t_n = intAndFilter::IMU_ptr->t;

    // euler's to get velocity
        // this is relative to the ROCKET body frame
        // and needs to be transformed to a flat earth body frame
        // see the gyro integration below
    intAndFilter::v_xB = v_x + a_x * (t_n - t_minus);
    intAndFilter::v_yB = v_y + a_y * (t_n - t_minus);
    intAndFilter::v_zB = v_z + a_z * (t_n - t_minus);


    //gyro integration also eulers
        //need <eigen> for matrix stuff
    intAndFilter::omega_x = omega_x + alpha_x * (t_n - t_minus);
    intAndFilter::omega_y = omega_y + alpha_y * (t_n - t_minus);
    intAndFilter::omega_z = omega_z + alpha_z * (t_n - t_minus);

    intAndFilter::theta_x = theta_x + omega_x * (t_n - t_minus);
    intAndFilter::theta_y = theta_y + omega_y * (t_n - t_minus);
    intAndFilter::theta_z = theta_z + omega_z * (t_n - t_minus);

        //this needs to use the transformation matrix from body to flat earth corrdinates
    intAndFilter::v_x = 0;    
    intAndFilter::v_y = 0;
    intAndFilter::v_z = 0;
    
    intAndFilter::t_minus = intAndFilter::t_n;
}
















