#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPL3115A2.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

//TESTING

void setup() {
  // put your setup code here, to run once:
    pinMode(13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, LOW); // RX LED on
  delay(333);
  digitalWrite(13, HIGH); // RX LED off
  delay(333);
}