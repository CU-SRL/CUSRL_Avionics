#include "yonics.hpp"

FlashOp::FlashOp() {
    /* 
    FlashOp::FlashOp()
    Unparameterized constructor
    Initializes flash pointer to NULL
    */
    flash = NULL;
}

FlashOp::FlashOp(SPIFlash* flash) {
    /*
    FlashOp::FlashOp(SPIFlash* flash)
    Parameterized Constructor

    Input:
        SPIFlash* flash --- pointer to SPIFlash object
    
    Initializes class member pointer flash to the input value.
    Calls begin() function from SPIFlash class.
    */
    this->flash = flash;

    if (!flash->begin()) {
        // IDEALLY DO SOMETHING HERE
    }

}

// ========== IMU ==========
bool FlashOp::setIMU(uint8_t size, float frequency) {
    size_IMU = size;
}

bool FlashOp::writeIMU(IMUdata* data) {
    uint32_t currAddr = nSamples_IMU*size_IMU + addr_start_IMU;
    nSamples_IMU++;

    if(flash->writeAnything(currAddr,data)) {return true;}
    else {return false;}
}

bool FlashOp::readIMU(IMUdata* data, int idx) {
    if (idx <= nSamples_IMU) {
        uint32_t currAddr = idx*size_IMU + addr_start_IMU;
        flash->readAnything(currAddr,data);
        return true;
    }
    else {return false;}
}
// =========================

void FlashOp::addWP(int pin) {
    /*
    void FlashOp::addWP(int pin)

    Add Write Protect pin for the flash chip.
    Drives WP pin high.
    */
    digitalWrite(pin,HIGH);
}