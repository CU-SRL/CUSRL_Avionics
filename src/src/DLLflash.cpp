#include "DLLflash.hpp"

DLLtype::DLLtype(void* refData,int dataSize,char* id) {
    this->refData = refData;
    this->dataSize = dataSize;
    strcpy(this->id,id);
}

int DLLtype::getDataSize() {
    return dataSize;
}

char* DLLtype::getID() {
    return id;
}

bool DLLtype::writeSample(uint32_t addr_write,SPIFlash* flash) 
{
}

DLLflash::DLLflash(int flashpin) 
{
    DLLflash::flash = new SPIFlash(flashpin);

    // Initialize flash chip
    if(!flash->begin())
    {
      while(true)
      {
        Serial.println("Failed to initialize the Flash Chip");
      }
    }
    DLLflash::flashsize = flash->getCapacity();
}

void DLLflash::addType(void* refData, int dataSize, char* id) {
    types.push_back(DLLtype(refData,dataSize,id));
}

bool DLLflash::writeSample(char* id) {

    // Loop through vector until finding the correct thing
    for (int i=0;i<types.size();i++) {
        if (strcmp(id,types[i].getID())) {
            bool returnVal = types[i].writeSample(addr_next_available,flash);
            if(returnVal) {addr_next_available += types[i].getDataSize();}
            return returnVal;
        }
    }
    return false;
}
