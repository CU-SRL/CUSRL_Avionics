#include "yonics.hpp"

HIGHG_ACCEL::HIGHG_ACCEL() {
    xPin = 0;
    yPin = 1;
    zPin = 2;
    bitDepth = 10;

    init();
}

HIGHG_ACCEL::HIGHG_ACCEL(int xPin, int yPin, int zPin) {
    // Define the x y z pin assignments and set the class members
    this->xPin = xPin;
    this->yPin = yPin;
    this->zPin = zPin;

    // Define the bit Depth at 10
    bitDepth = 10;

    // Set offset and ratio based on bitDepth
    init();
}

HIGHG_ACCEL::HIGHG_ACCEL(int xPin, int yPin, int zPin, bool highBitDepth) {
    // Define the x y z pin assignments and set the class members
    this->xPin = xPin;
    this->yPin = yPin;
    this->zPin = zPin;

    // Define the bit depth at 16
    bitDepth = 16;

    // If highBitDepth set the analog read resolution to 16 (MAX)
    if (highBitDepth) {analogReadRes(16);}

    // Set offset and ratio based on bitDepth
    init();
}

void HIGHG_ACCEL::init() {
    offset = 0.5*(2^bitDepth);
    ratio = (float)offset/(float)(2*maxG);
}

void HIGHG_ACCEL::sample(ACCELdata* data) {
    data->t = millis();
    data->x = formatVal(analogRead(xPin));
    data->y = formatVal(analogRead(yPin));
    data->z = formatVal(analogRead(zPin));


    /*Serial.print(formatVal(analogRead(xPin)));
    Serial.print(" ");
    Serial.print(formatVal(analogRead(yPin)));
    Serial.print(" ");
    Serial.print(formatVal(analogRead(zPin)));
    Serial.println(" ");*/
}

float HIGHG_ACCEL::formatVal(int rawVal) {
    rawVal = rawVal - offset;
    return (float)rawVal*ratio;
}