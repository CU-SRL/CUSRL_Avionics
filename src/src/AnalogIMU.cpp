#include "yonics.hpp"

AnalogIMU::AnalogIMU() {
    xPin = 0;
    yPin = 1;
    zPin = 2;
    bitDepth = 10;

    init();
}

AnalogIMU::AnalogIMU(int xPin, int yPin, int zPin) {
    this->xPin = xPin;
    this->yPin = yPin;
    this->zPin = zPin;
    bitDepth = 10;

    init();
}

AnalogIMU::AnalogIMU(int xPin, int yPin, int zPin, bool highBitDepth) {
    this->xPin = xPin;
    this->yPin = yPin;
    this->zPin = zPin;
    bitDepth = 10;

    if (highBitDepth) {
    analogReadRes(16);
    }

    init();
}

void AnalogIMU::init() {
    offset = 0.5*(2^bitDepth);
    ratio = (float)offset/(float)(2*maxG);
}

void AnalogIMU::sample(Acceldata* data) {
    data->t = millis();
    data->x = formatVal(analogRead(xPin));
    data->y = formatVal(analogRead(yPin));
    data->z = formatVal(analogRead(zPin));
}

float AnalogIMU::formatVal(int rawVal) {
    rawVal = rawVal - offset;
    return (float)rawVal*ratio;
}