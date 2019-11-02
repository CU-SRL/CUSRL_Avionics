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
        Serial.println("Yikes. Flash chip didn't initialize.");
    }

}

// ========== IMU ==========
bool FlashOp::setIMU(uint8_t size, float frequency) {
    size_IMU = size;
    freq_IMU = frequency;
    addr_start_IMU = flash->readULong(0);

    Serial.print("IMU start address: ");
    Serial.println(addr_start_IMU);

    return true;
}

bool FlashOp::writeIMU(IMUdata* data) {
    if (flash->writeAnything(addr_curr_IMU,data)) {
        flash->readAnything(addr_start_IMU,data);
        Serial.println(data->t);
        addr_curr_IMU+=size_IMU;
        return true;
    }
    else {return false;}
}

IMUdata* FlashOp::readIMU(uint32_t idx) {
    uint32_t currAddr = idx*size_IMU + addr_start_IMU;

    if(!flash->writeByte(currAddr,0)) {
        flash->readAnything(currAddr,tempIMU);

        Serial.printf("Data: %.4f\n",tempIMU.orient_euler[1]);

        return &tempIMU;
    }
    else{
        return NULL;
    }
}
// =========================

// ========== BAROM ==========
bool FlashOp::setBAROM(uint8_t size, float frequency) {
    size_BAROM = size;
    freq_BAROM = frequency;
    addr_start_BAROM = flash->readULong(4);

    Serial.print("BAR0M start address: ");
    Serial.println(addr_start_BAROM);

    return true;
}

bool FlashOp::writeBAROM(BAROMdata* data) {
    if (flash->writeAnything(addr_curr_BAROM,data)) {
        addr_curr_BAROM+=size_BAROM;
        return true;
    }
    else {return false;}
}

BAROMdata* FlashOp::readBAROM(uint32_t idx) {
    uint32_t currAddr = idx*size_BAROM + addr_start_BAROM;
    
    if(!flash->writeByte(currAddr,0)) {
        flash->readAnything(currAddr,tempBAROM);
        return &tempBAROM;
    }
    else{
        return NULL;
    }
}
// =========================

// ========== ACCEL ==========
bool FlashOp::setACCEL(uint8_t size, float frequency) {
    size_ACCEL = size;
    freq_ACCEL = frequency;
    addr_start_ACCEL = flash->readULong(8);

    return true;
}

bool FlashOp::writeACCEL(ACCELdata* data) {
    
    if (flash->writeAnything(addr_curr_ACCEL,data)) {
        addr_curr_ACCEL+=size_ACCEL;
        return true;
    }
    else {return false;}
}

ACCELdata* FlashOp::readACCEL(uint32_t idx) {
    uint32_t currAddr = idx*size_ACCEL + addr_start_ACCEL;
    
    if(!flash->writeByte(currAddr,0)) {
        flash->readAnything(currAddr,tempACCEL);
        return &tempACCEL;
    }
    else{
        return NULL;
    }
}
// =========================

// ========== GPS ==========
bool FlashOp::setGPS(uint8_t size, float frequency) {
    size_GPS = size;
    freq_GPS = frequency;
    addr_start_GPS = flash->readULong(12);

    return true;
}

bool FlashOp::writeGPS(GPSdata* data) {
    if (flash->writeAnything(addr_curr_GPS,data)) {
        addr_curr_GPS+=size_GPS;
        return true;
    }
    else {return false;}
}

GPSdata* FlashOp::readGPS(uint32_t idx) {
    uint32_t currAddr = idx*size_GPS + addr_start_GPS;
    
    if(!flash->writeByte(currAddr,0)) {
        flash->readAnything(currAddr,tempGPS);
        return &tempGPS;
    }
    else{
        return NULL;
    }
}
// =========================

bool FlashOp::startWriting() {

    // Erase flash chip
    flash->eraseChip();

    uint32_t chipSize,startOffset;

    // Start offset: address offset because first few store other info
    startOffset = 16;

    // Get size of flash chip
    chipSize = flash->getCapacity() - startOffset;

    // Bytes per second for each data type
    float rate_IMU = (float)size_IMU*freq_IMU;
    float rate_BAROM = (float)size_BAROM*freq_BAROM;
    float rate_ACCEL = (float)size_ACCEL*freq_ACCEL;
    float rate_GPS = (float)size_GPS*freq_GPS;
    float rate_total = rate_IMU + rate_BAROM + rate_ACCEL + rate_GPS;

    // Print rates to Serial
    Serial.printf("IMU   rate: %.3f\n",rate_IMU);
    Serial.printf("BAROM rate: %.3f\n",rate_BAROM);
    Serial.printf("ACCEL rate: %.3f\n",rate_ACCEL);
    Serial.printf("GPS   rate: %.3f\n",rate_GPS);

    // Fractional share of each data type
    float share_IMU = rate_IMU/rate_total;
    float share_BAROM = rate_BAROM/rate_total;
    float share_ACCEL = rate_ACCEL/rate_total;
    float share_GPS = rate_GPS/rate_total;

    // Print shares to Serial
    Serial.printf("IMU   share: %.3f\n",share_IMU);
    Serial.printf("BAROM share: %.3f\n",share_BAROM);
    Serial.printf("ACCEL share: %.3f\n",share_ACCEL);
    Serial.printf("GPS   share: %.3f\n",share_GPS);

    // Starting address of each type
    addr_start_IMU = startOffset;
    addr_start_BAROM = startOffset + (uint32_t)(share_IMU*chipSize);
    addr_start_ACCEL = addr_start_BAROM + (uint32_t)(share_BAROM*chipSize);
    addr_start_GPS = addr_start_ACCEL + (uint32_t)(share_ACCEL*chipSize);

    // Write starting addresses to flash chip
    flash->writeULong(0,addr_start_IMU);
    flash->writeULong(4,addr_start_BAROM);
    flash->writeULong(8,addr_start_ACCEL);
    flash->writeULong(12,addr_start_GPS);

    // Set current addresses to starting addresses
    addr_curr_IMU = addr_start_IMU;
    addr_curr_BAROM = addr_start_BAROM;
    addr_curr_ACCEL = addr_start_ACCEL;
    addr_curr_GPS = addr_start_GPS;

    // Print starting addresses to Serial
    Serial.print("IMU   start address: ");
    Serial.println(addr_start_IMU);
    Serial.print("BAROM start address: ");
    Serial.println(addr_start_BAROM);
    Serial.print("ACCEL start address: ");
    Serial.println(addr_start_ACCEL);
    Serial.print("GPS   start address: ");
    Serial.println(addr_start_GPS);

    writing = true;

    return true;
}

void FlashOp::addWP(int pin) {
    /*
    void FlashOp::addWP(int pin)

    Add Write Protect pin for the flash chip.
    Drives WP pin high.
    */
    digitalWrite(pin,HIGH);
}