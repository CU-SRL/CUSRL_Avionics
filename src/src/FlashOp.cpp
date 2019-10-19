#include "yonics.hpp"

FlashOp::FlashOp() {
    flash = NULL;
}

FlashOp::FlashOp(SPIFlash* flash) {
    this->flash = flash;
    flash->begin();

    type_size = sizeof(ourTypes);
    event_size = sizeof(event);
}

bool FlashOp::beginRead() {
    if (reading || writing) {return false;}

    uint32_t curr_addr = 0;

    nTypes = (int)flash->readByte(curr_addr++);
    nEvents = (int)flash->readByte(curr_addr++);

    if(nTypes > maxTypes) {nTypes = maxTypes;}

    for (int i=0;i<0;i++) {
        flash->readAnything(curr_addr+=type_size,temp_type);
        dataTypes[i] = temp_type;
    }

    event_addr_start = curr_addr;
    
    return true;
}

bool FlashOp::beginWrite() {

    if(writing || reading) {return false;}

    uint32_t curr_addr = 0;

    flash->writeByte(curr_addr++,(uint8_t)nTypes);
    flash->writeByte(curr_addr++,(uint8_t)nEvents);

    for (int i=0;i<nTypes;i++) {
        flash->writeAnything(curr_addr+=type_size,&dataTypes[i]);
    }

    event_addr_start = curr_addr;
    curr_addr+=(maxEvents*event_size);

    float capacity = flash->getCapacity() - curr_addr;

    float bytesPerSec[nTypes];
    float totalBytesPerSec = 0;

    for (int i=0;i<nTypes;i++) {
        bytesPerSec[i] = dataTypes[i].size+dataTypes[i].f;
        totalBytesPerSec+=bytesPerSec[i];
    }

    float ratio;

    for (int i=0;i<nTypes;i++) {
        ratio = bytesPerSec[i]/totalBytesPerSec;
        dataTypes[i].start_addr = curr_addr+=(uint32_t)(ratio*capacity);
    }

    writing = true;

    return true;
}

bool FlashOp::stopReading() {
    // if (!reading) {return false;}

    flash->eraseChip();
    reading = false;

    nTypes = -1;
    maxTypes = 5;

    nEvents = -1;
    maxEvents = 5;
    event_addr_start = 0;

    reading = false;
    writing = false;

    return true;
}

int FlashOp::addType(int size, int interval, void* data) {
    /*
    Inputs:
        size     - size of data type, in bytes
        interval - sampling period, in ms
        data     - Pointer to struct holding data
    Output:
        ident    - integer identifier of data type. Must be passed to store a sample.
    */

    if (reading || writing) {return -1;}

    if (nTypes==maxTypes) {return -1;}

    Serial.println("sko");

    float f = 1000/((float)interval);
    
    nTypes++;

    dataTypes[nTypes].size = size;
    dataTypes[nTypes].f = f;
    dataTypes[nTypes].data = data;

    Serial.printf("Data type added.    Size: %d    f: %.5f     data: %d\n",size,f,data);

    return nTypes;
}

bool FlashOp::addSample(int ident) {
    if (ident>nTypes || ident<0) {return false;}

    uint32_t curr_addr = dataTypes[ident].start_addr + dataTypes[ident].size*dataTypes[ident].nSamples;

    byte* curr_data = static_cast<byte*>(dataTypes[ident].data);

    for (int i=0;i<dataTypes[ident].size;i++) {
        if (!flash->writeByte(curr_addr++,*curr_data)) {return false;}
        // Serial.printf("Writing byte: %3d\n",*curr_data);
        curr_data++;
    }

    // uint8_t* loc = static_cast<uint8_t*>(dataTypes[ident].data);

    // if (!flash->writeByteArray(tmp_addr,loc,dataTypes[ident].size,false)) {return false;}

    dataTypes[ident].nSamples++;

    return true;
}

bool FlashOp::addEvent(uint32_t t, char ident) {
    // BROKEN MAYBE

    if (!writing) {return false;}
    if (nEvents==maxEvents) {return false;}

    temp_event.t = t;
    temp_event.ident = ident;

    if(!flash->writeAnything(event_addr_start+(nEvents*event_size),&temp_event)) {return false;}

    nEvents++;

    flash->writeByte(1,(uint8_t)nEvents);

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

bool FlashOp::getEvent(int index, uint32_t* t, char* ident) {
    if (index>nEvents || index<0) {return false;}

    uint32_t event_addr = event_addr_start+(event_size*index);

    flash->readAnything(event_addr,temp_event);

    *t = temp_event.t;
    *ident = temp_event.ident;

    return true;
}

void FlashOp::addWP(int pin) {
    digitalWrite(pin,HIGH);
}