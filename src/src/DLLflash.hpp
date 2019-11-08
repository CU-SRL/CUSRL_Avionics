#include <Arduino.h>
#include<vector>
#include "StructDefs.hpp"
#include "SPIMemory.h"

struct Node {
    uint32_t index = -1;
    uint32_t next = -1;
    uint32_t prev = -1;
    void* data = NULL;
    int dataSize = 0;
};

class DLLtype {
    private:
        Node currNode;
        Node nextNode;
        void* refData = NULL;
        uint32_t head = -1;
        uint32_t tail = -1;
        int dataSize = 0;
        char id[3];
    public:
        DLLtype(void*,int,char*);
        bool writeSample(uint32_t,SPIFlash*);
        int getDataSize();
        char* getID();


};

class DLLflash {
    private:
        // Next available address
        uint32_t addr_next_available = 0;
        std::vector <DLLtype> types;
        SPIFlash* flash = NULL;
    public:
        DLLflash();
        void addType(void*,int,char*);
        bool writeSample(char*);

};