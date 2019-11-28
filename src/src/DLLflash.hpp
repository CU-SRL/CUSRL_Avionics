#include <Arduino.h>
#include<vector>
#include "StructDefs.hpp"
#include "SPIMemory.h"

class DLLtype {
    private:
        void* prevBuffer = NULL; // Buffer of previous sample
        void* currBuffer = NULL; // Buffer of current sample
        void* writeBuffer = NULL; // Buffer of data to be written
        void* refData = NULL; // Pointer to struct containing data
        uint32_t head = -1; // Flash chip addr of head
        uint32_t tail = -1; // Flash chip addr of tail
        int dataSize = 0; // Size of one sample (i.e. size of refData)
        int addrSize; // Size of one flash chip address
        char id[3];  // Three-character identifier (e.g. IMU)

        bool init();
    public:
        DLLtype();
        DLLtype(void*,int);
        ~DLLtype();
        bool setType(void*,int);
        int writeSample(uint32_t,SPIFlash*);
        bool readSample();
        char* getID();


};

class DLLflash {
    private:
        // Next available address
        uint32_t addr_next_available = 0;
        std::vector <DLLtype*> types;
        SPIFlash* flash = NULL;
    public:
        DLLflash();
        ~DLLflash();
        template <class T>
        void addType(T*,char*);
        bool writeSample(char*);
};
