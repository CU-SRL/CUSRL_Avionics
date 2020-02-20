/*
==================================================
================    Carter Mak   =================
================ CU SRL Avionics =================
================  November, 2019 =================
==================================================
*/

#include "DLLflash.hpp"

DLLtype::DLLtype(void* dataPtr,int dataSize,char* id) {
    /*
    Parameterized constructor
    */

    // Set ID
    strcpy(this->id,id);

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

void DLLtype::bufferFirstSample() {
    memcpy(nextBuffer,refData,dataSize);
}

bool DLLtype::buffer2flash(uint32_t writeAddr,SPIFlash* flash) {
    
}

bool DLLtype::setType(void* dataPtr,int dataSize) {
    // Check whether type is already set
    if (dataSize) {return false;}

    // Set values
    refData = dataPtr;
    this->dataSize = dataSize;

    return true;
}

bool DLLtype::writeSample(uint32_t next,SPIFlash* flash) {
    /*
    TODO: The rest of this function
    - Error checking
    - Correctly assign the head/tail addresses
    - Copy over the correct data

    */

    this->next = next;
    uint32_t writeAddr = curr;

    // Write next address
    flash->writeULong(writeAddr,next);
    writeAddr+=addrSize;

    // Write prev address
    flash->writeULong(writeAddr,prev);
    writeAddr+=addrSize;

    for (int i=0;i<dataSize;i++) {
        flash->writeByte(writeAddr+=1,*(uint8_t*)(currBuffer+i));
    }
    
    // Copy next to curr
    memcpy(currBuffer,nextBuffer,dataSize);

    // Copy data from reference pointer to next
    memcpy(nextBuffer,refData,dataSize);

    return true;
}

char* DLLtype::getID() {
    return id;
}

DLLflash::DLLflash(int flashpin) 
{
    // Tell SPIFlash class to yeet the flash chip into action
    flash = new SPIFlash(flashpin);

    // Initialize flash chip
    if(!flash->begin())
    {
        // If the flash chip doesn't successfully initialize
        while(true)
        {
            // Absolutely spam over Serial, and also don't do anything
            Serial.println("Failed to initialize the Flash Chip");
        }
    }
    // Store the size of the flash chip to flashSize
    flashSize = flash->getCapacity();
}

DLLflash::~DLLflash() {
    // Free memory
    for (std::vector<DLLtype*>::iterator it = types.begin();it!=types.end();it++) {
        delete *it;
        *it = nullptr;
    }
}

template <class T>
void DLLflash::addType(T* data,char* id) {

    int dataSize = sizeof(*data);
    void* dataPtr = (void*) data;

    DLLtype* newType = new DLLtype(dataPtr,dataSize,id);
    types.push_back(newType);
}

bool DLLflash::writeSample(char*) 
{
    // Loop over vector of data types until finding the one with the correct ID
    // for () {}
}