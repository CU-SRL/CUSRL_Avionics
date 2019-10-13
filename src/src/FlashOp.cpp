#include "yonics.hpp"

FlashOp::FlashOp(SPIFlash* flash) {
    this->flash = flash;
    this->flash->begin();
}

int FlashOp::addType(int size, float f, void* data) {
    /*
    Inputs:
        size - size of data type, in bytes
        f    - sampling frequency, in hertz
        data - Pointer to struct holding data
    Output:
        ident - integer identifier of data type. Must be passed to store a sample.
    */

    if (running) {return -1;}

    if (nTypes==maxTypes) {return -1;}
    
    nTypes++;

    dataTypes[nTypes].size = size;
    dataTypes[nTypes].f = f;
    dataTypes[nTypes].data = data;

    return nTypes;
}

bool FlashOp::addSample(int ident) {
    if (ident>nTypes || ident<0) {return false;}

    uint32_t tmp_addr = dataTypes[ident].start_addr + dataTypes[ident].size*dataTypes[ident].nSamples;

    uint8_t* loc = static_cast<uint8_t*>(dataTypes[ident].data);

    if (!flash->writeByteArray(tmp_addr,loc,dataTypes[ident].size,false)) {return false;}

    dataTypes[ident].nSamples++;

    return true;
}

bool FlashOp::addEvent(uint32_t t, char ident) {

    if (!running) {return false;}
    if (nEvents==maxEvents) {return false;}

    nEvents++;

    events[nEvents].t = t;
    events[nEvents].ident = ident;

    return true;

}

bool FlashOp::getType(int ident, int* size) {
    if (ident>nTypes || ident<0) {return false;}
    *size = dataTypes[ident].size;

    return true;
}

bool FlashOp::getSample(int ident, int sample, void* data) {
    if (ident>nTypes || ident<0) {return false;}
    if (dataTypes[ident].nSamples<sample) {return false;}

    uint32_t flash_addr = dataTypes[ident].start_addr + dataTypes[ident].size*dataTypes[ident].nSamples;
    uint8_t* data_addr = static_cast<uint8_t*>(data);

    for (int i=0;i<dataTypes[ident].size;i++) {
        *(data_addr+i) = flash->readByte(flash_addr++);
    }

    return true;
}