#include "yonics.hpp"
#include<vector>



struct Node {
    uint32_t index = -1;
    uint32_t next = -1;
    uint32_t prev = -1;
};

class DLLtype {
    public:
        uint32_t head = -1;
        uint32_t tail = -1;

};

class DLLflash {
    private:
        // Next available address
        uint32_t addr_next_available = 0;
    public:
        DLLflash();

};