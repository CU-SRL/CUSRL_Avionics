#include "yonics.hpp"

DigitalBAROM::DigitalBAROM() {}

bool DigitalBAROM::begin() {
    return BAROM.begin();
}

void DigitalBAROM::sample(BAROMdata* data) {
    // Write data to struct
    data->altitude = BAROM.getAltitude();
    data->pressure = BAROM.getPressure();
    data->temperature = BAROM.getTemperature();
    data->t = millis();
}