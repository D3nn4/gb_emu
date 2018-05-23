#ifndef _BINARYINSTRUCTIONS_
#define _BINARYINSTRUCTIONS_

#include <functional>
#include <map>
#include "imemory.hpp"

typedef std::function<int(IMemory&)> funcInstruct;

using namespace std::placeholders;

class BinaryInstructions 
{
public:

    int checkBit7InH(IMemory& memory);

    std::map<uint8_t, funcInstruct> _binaryInstructions =
        {
            {0x7c, std::bind(&BinaryInstructions::checkBit7InH, this, _1)},
        };

private:

    void checkBitInRegister(int bit, IMemory::REG8BIT reg, IMemory& memory);

};
#endif /*IBINARYINSTRUCTIONS*/
