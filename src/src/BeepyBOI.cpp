#include "yonics.hpp"

BeepyBOI::BeepyBOI() {
    pin = 2;
}

BeepyBOI::BeepyBOI(int pin) {
    this->pin = pin;
}

void BeepyBOI::hello() {
    tone(pin,midTone,200); // hehe concert A
    delay(1000);
}

void BeepyBOI::error() {
    tone(pin,errTone,4000);
    delay(5000);
}

void BeepyBOI::countdown(int s) {
    for(int i=0;i<s;i++) {
        tone(pin,midTone,20);
        delay(1000);
    }
}

void BeepyBOI::lowBeep() {
    tone(pin,lowTone,500);
    delay(500);
}

void BeepyBOI::midBeep() {
    tone(pin,midTone,500);
    delay(500);
}

void BeepyBOI::hiBeep() {
    tone(pin,hiTone,500);
    delay(500);
}

void BeepyBOI::bombBeep() {
    for(int i=0;i<10;i++) {
        tone(pin,hiTone,25);
        delay(50);
    }
    delay(250);
    tone(pin,2*hiTone,250);
    delay(250);
}