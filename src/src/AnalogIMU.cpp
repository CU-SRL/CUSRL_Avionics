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
<<<<<<< HEAD
=======
    bitDepth = 16;
>>>>>>> 51ff3efac751bb77e930d8d1d45736bd63eb3926

    if (highBitDepth) {
        analogReadRes(16);
        bitDepth = 16;
    }
    else {
        bitDepth = 10;
    }

    init();
}

void AnalogIMU::init() {
    offset = 0.5*(2^bitDepth);
    ratio = (float)offset/(float)(2*maxG);
}

void AnalogIMU::sample(ACCELdata* data) {
    data->t = millis();
    data->x = formatVal(analogRead(xPin));
    Serial.print("X: ");
    Serial.print(formatVal(analogRead(xPin)));

    data->y = formatVal(analogRead(yPin));
    Serial.print(" Y: ");
    Serial.print(formatVal(analogRead(yPin)));

    data->z = formatVal(analogRead(zPin));
<<<<<<< HEAD
    Serial.print(" Z: ");
    Serial.println(formatVal(analogRead(zPin)));
=======


    /*Serial.print(formatVal(analogRead(xPin)));
    Serial.print(" ");
    Serial.print(formatVal(analogRead(yPin)));
    Serial.print(" ");
    Serial.print(formatVal(analogRead(zPin)));
    Serial.println(" ");*/
>>>>>>> 51ff3efac751bb77e930d8d1d45736bd63eb3926
}

float AnalogIMU::formatVal(int rawVal) {
    rawVal = rawVal - offset;
    return (float)rawVal*ratio;
}