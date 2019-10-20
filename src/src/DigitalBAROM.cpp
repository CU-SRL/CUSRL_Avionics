#include "yonics.hpp"

DigitalBAROM::DigitalBAROM() {}

bool DigitalBAROM::begin() {
    return BAROM.begin();
}

void DigitalBAROM::sample(BAROMdata* data) {
    // Write data to struct
    data->altitude = BAROM.getAltitude();

    Serial.print(BAROM.getAltitude());
    Serial.print(" ");

    data->pressure = BAROM.getPressure();

    Serial.print(BAROM.getPressure());
    Serial.print(" ");

    data->temperature = BAROM.getTemperature();

    Serial.print(BAROM.getTemperature());
    Serial.println(" ");

    data->t = millis();
}