#include "yonics.hpp"

////////////////////////////////////
//
//  pyro 1 and 2 are always drogue
//
//  pyro 3 and 4 are always main
//
//  for a staging charge, we'll need more ports
//
////////////////////////////////////



pyroPorts::pyroPorts(int fireAlt, BAROMdata* BAROM_ptr)
{
    pyroPorts::fireAlt = fireAlt;

    pyroPorts::BAROM_ptr = BAROM_ptr;


    pinMode(pyro1, OUTPUT);
    pinMode(pyro2, OUTPUT);
    pinMode(pyro3, OUTPUT);
    pinMode(pyro4, OUTPUT); // do these need to be like pyroPorts::pyro4  ??
}


//pyro 1 and 2
void pyroPorts::fireAtApogee(){

    //pulling from integrated+filtered accel data

    if (/* the velocity == 0*/){

        digitalWrite(pyro1, HIGH);
        digitalWrite(pyro2, HIGH)
    }

}


// Pyro 3 and 4
void pyroPorts::fireAtAlt(){

    //pulling from the barameter
        if(pyroPorts::BAROM_ptr->altitude < fireAlt){
            digital.write(pyro3, HIGH);
            digital.write(pyro4, HIGH);
        }
}




