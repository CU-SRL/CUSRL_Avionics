#include <Arduino.h>
#include<vector>
#include "StructDefs.hpp"
#include "SPIMemory.h"

template <class T>
class DLLtype {
    private:
        T prevData;
        T currData;
        T* refData = NULL;
        uint32_t head = -1;
        uint32_t tail = -1;
        int dataSize = 0;
    public:
        DLLtype();
        DLLtype(T*);
        bool writeSample(uint32_t,SPIFlash*);
        int getDataSize();
        char* getID();


};

template <class T>
class DLLflash {
    private:
        // Next available address
        uint32_t addr_next_available = 0;
        std::vector <DLLtype*> types;
        SPIFlash* flash = NULL;
    public:
        DLLflash();
        void addType(void*,int,char*);
        bool writeSample(char*);

};