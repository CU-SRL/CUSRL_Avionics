#include "DLLflash.hpp"

DLLtype::DLLtype() {
    dataSize = sizeof(T);
}

DLLtype::DLLtype(T* data) {
    refData = data;
    dataSize = sizeof(T);
}

int DLLtype::getDataSize() {
    return dataSize;
}

char* DLLtype::getID() {
    return id;
}

bool DLLtype::writeSample(uint32_t addr_write,SPIFlash* flash) {
    
}
