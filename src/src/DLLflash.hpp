/*
==================================================
================    Carter Mak   =================
================ CU SRL Avionics =================
================  November, 2019 =================
==================================================
*/

#include <Arduino.h>
#include<vector>
#include "StructDefs.hpp"
#include "SPIMemory.h"

/*
class DLLtype
-------------

Class definition to store, manage, and operate a single data type being stored to flash memory
*/
class DLLtype {
    private:
        void* nextBuffer = NULL; // Buffer of next sample
        void* currBuffer = NULL; // Buffer of current sample
        void* refData = NULL; // Pointer to struct containing data

        uint32_t head = 0; // Flash chip addr of head
        uint32_t tail = 0; // Flash chip addr of tail

        uint32_t prev = 0; // Address on flash chip of prev sample
        uint32_t curr = 0; // Address on flash chip of curr sample
        uint32_t next = 0; // Address on flash chip allocated for next sample
        int nSamplesWritten = 0; // Number of samples stored to flash chip


        int dataSize = 0; // Size of one sample (i.e. size of refData)
        int addrSize; // Size of one flash chip address
        char id[3];  // Three-character identifier (e.g. IMU)

        bool init();
        void bufferFirstSample();
        bool buffer2flash(uint32_t,SPIflash*);
    public:
        DLLtype(void*,int,char*);
        ~DLLtype();
        bool setType(void*,int);
        bool writeSample(uint32_t,SPIFlash*);
        bool readSample();
        char* getID();


};

/*
class DLLflash
--------------

Class to manage and operate all flash memory interactions
*/
class DLLflash {
    private:
        uint32_t addr_next_available = 0; // Next available address
        std::vector <DLLtype*> types; // Vector of instantiated types
        SPIFlash* flash = NULL; // Pointer to SPIflash object (flash chip)
    public:
        DLLflash();
        ~DLLflash();
        template <class T>
        void addType(T*,char*);
        bool writeSample(char*);
};
