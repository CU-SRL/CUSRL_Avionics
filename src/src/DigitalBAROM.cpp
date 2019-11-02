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

    /*Serial.print("Altitude: ");
    Serial.print(BAROM.getAltitude());
    Serial.print(" Pressure: ");
    Serial.print(BAROM.getPressure());
    Serial.print(" Temperature: ");
    Serial.println(BAROM.getTemperature());*/

    data->t = millis();
}