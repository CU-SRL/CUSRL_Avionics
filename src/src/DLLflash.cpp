#include "DLLflash.hpp"

DLLtype::DLLtype() {
    addrSize = sizeof(uint32_t);
}

DLLtype::DLLtype(void* dataPtr,int dataSize) {
    // Set values
    refData = dataPtr;
    this->dataSize = dataSize;

    addrSize = sizeof(uint32_t);
}

DLLtype::~DLLtype() {
    free(nextBuffer);
    free(currBuffer);
}

bool DLLtype::init() {
    /*
    Allocates memory for the data buffers using the dataSize member. Fails if the buffers have already been allocated.
    */

    // If data size hasn't been initialized, return false
    if (~dataSize) {return false;}

    // If data buffers have already been allocated, return false
    if (nextBuffer || currBuffer) {return false;}

    // Check that dataSize is populated with something reasonable
    if (dataSize>1000) {return false;}

    // Allocate buffers
    nextBuffer = malloc(dataSize);
    currBuffer = malloc(dataSize);

    return true;
}

bool DLLtype::setType(void* dataPtr,int dataSize) {
    // Check whether type is already set
    if (dataSize) {return false;}

    // Set values
    refData = dataPtr;
    this->dataSize = dataSize;

    return true;
}

int DLLtype::writeSample(uint32_t addrNext,SPIFlash* flash) {
    /*
    TODO: The rest of this function
    - Correctly assign the head/tail addresses
    - Copy over the correct data

    Returns: Size to be allocated for next sample
    */

    uint32_t curr = tail;

    // Write next address
    flash->writeULong(curr,addrNext);
    curr+=addrSize;

    // Write prev address
    flash->writeULong(curr,0); // TODO: not sure what to write here
    curr+=addrSize;

    for (int i=0;i<dataSize;i++) {
        flash->writeByte(curr+=1,*(uint8_t*)(currBuffer+i));
    }
    

    return true;
}

char* DLLtype::getID() {
    return id;
}