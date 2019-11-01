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
    freq_IMU = frequency;
    addr_start_IMU = flash->readULong(0);
    nSamples_IMU = flash->readULong(4);
}

bool FlashOp::writeIMU(IMUdata* data) {
    uint32_t currAddr = nSamples_IMU*size_IMU + addr_start_IMU;
    nSamples_IMU++;

    if(flash->writeAnything(currAddr,data)) {return true;}
    else {return false;}
}

bool FlashOp::readIMU(IMUdata* data, int idx) {
    if (idx>=0 && idx <= nSamples_IMU) {
        uint32_t currAddr = idx*size_IMU + addr_start_IMU;
        flash->readAnything(currAddr,data);
        return true;
    }
    else if (idx<0 && (-idx) <= nSamples_IMU) {
        uint32_t currAddr = -idx*size_IMU + addr_start_IMU + size_IMU*nSamples_IMU;
        flash->readAnything(currAddr,data);
        return true;
    }
    else {return false;}
}
// =========================

// ========== BAROM ==========
bool FlashOp::setBAROM(uint8_t size, float frequency) {
    size_BAROM = size;
    freq_BAROM = frequency;
    addr_start_BAROM = flash->readULong(8);
    nSamples_BAROM = flash->readULong(12);
}

bool FlashOp::writeBAROM(BAROMdata* data) {
    uint32_t currAddr = nSamples_BAROM*size_BAROM + addr_start_BAROM;
    nSamples_BAROM++;

    if(flash->writeAnything(currAddr,data)) {return true;}
    else {return false;}
}

bool FlashOp::readBAROM(BAROMdata* data, int idx) {
    if (idx <= nSamples_BAROM) {
        uint32_t currAddr = idx*size_BAROM + addr_start_BAROM;
        flash->readAnything(currAddr,data);
        return true;
    }
    else if (idx<0 && (-idx) <= nSamples_BAROM) {
        uint32_t currAddr = -idx*size_BAROM + addr_start_BAROM + size_BAROM*nSamples_BAROM;
        flash->readAnything(currAddr,data);
        return true;
    }
    else {return false;}
}
// =========================

// ========== ACCEL ==========
bool FlashOp::setACCEL(uint8_t size, float frequency) {
    size_ACCEL = size;
    freq_ACCEL = frequency;
    addr_start_ACCEL = flash->readULong(16);
    nSamples_ACCEL = flash->readULong(20);
}

bool FlashOp::writeACCEL(ACCELdata* data) {
    uint32_t currAddr = nSamples_ACCEL*size_ACCEL + addr_start_ACCEL;
    nSamples_ACCEL++;

    if(flash->writeAnything(currAddr,data)) {return true;}
    else {return false;}
}

bool FlashOp::readACCEL(ACCELdata* data, int idx) {
    if (idx <= nSamples_ACCEL) {
        uint32_t currAddr = idx*size_ACCEL + addr_start_ACCEL;
        flash->readAnything(currAddr,data);
        return true;
    }
    else if (idx<0 && (-idx) <= nSamples_ACCEL) {
        uint32_t currAddr = -idx*size_ACCEL + addr_start_ACCEL + size_ACCEL*nSamples_ACCEL;
        flash->readAnything(currAddr,data);
        return true;
    }
    else {return false;}
}
// =========================

bool FlashOp::startWriting() {

    // Erase flash chip
    flash->eraseChip();

    uint32_t chipSize,startOffset;

    // Start offset: address offset because first few store other info
    startOffset = 32;

    // Get size of flash chip
    chipSize = flash->getCapacity - startOffset;

    // Bytes per second for each data type
    uint rate_IMU,rate_BAROM,rate_ACCEL,rate_total;
    rate_IMU = size_IMU*freq_IMU;
    rate_BAROM = size_BAROM*freq_BAROM;
    rate_ACCEL = size_ACCEL*freq_BAROM;
    rate_total = rate_IMU + rate_BAROM + rate_ACCEL;

    // Fractional share of each data type
    float share_IMU,share_BAROM,share_ACCEL;
    share_IMU = (float)rate_IMU/(float)rate_total;
    share_BAROM = (float)rate_BAROM/(float)rate_total;
    share_ACCEL = (float)rate_ACCEL/(float)rate_total;

    // Starting address of each type
    addr_start_IMU = startOffset;
    addr_start_BAROM = startOffset + (uint32_t)(share_IMU*chipSize);
    addr_start_ACCEL = addr_start_BAROM + (uint32_t)(share_BAROM*chipSize);

    // Re-initialize all sample counters to zero
    nSamples_IMU = 0;
    nSamples_BAROM = 0;
    nSamples_ACCEL = 0;

}

void FlashOp::addWP(int pin) {
    /*
    void FlashOp::addWP(int pin)

    Add Write Protect pin for the flash chip.
    Drives WP pin high.
    */
    digitalWrite(pin,HIGH);
}