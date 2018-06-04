#ifndef _BINARYINSTRUCTIONS_
#define _BINARYINSTRUCTIONS_

#include <functional>
#include <map>
#include "imemory.hpp"
#include "instructiondata.hpp"


using namespace std::placeholders;

class BinaryInstructions
{
public:

    int checkBit7InH(IMemory& memory);

    std::map<uint8_t, InstructionData> _instructions =
        {
            {0x7c, {4, std::bind(&BinaryInstructions::checkBit7InH, this, _1)} }
        };

private:

    void checkBitInRegister(int bit, IMemory::REG8BIT reg, IMemory& memory);

};
#endif /*IBINARYINSTRUCTIONS*/
