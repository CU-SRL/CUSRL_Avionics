/*
Carter Mak
September 21, 2019
University of Colorado, Boulder --- Sounding Rocket Laboratory
---
Description:
Test script using the Marzogh v3.2.0 SPI Memory library to read and write test data to-from our Teensy 3.6 and MT25QL256ABA8ESF-0SIT TR (32MB) flash chip.

Pinout:
MISO - 12
MOSI - 11
SCK  - 13
CS   - 29
*/

#include <Arduino.h>
#include <SPIFlash.h>    //get it here: https://github.com/LowPowerLab/SPIFlash
#include <SPI.h>