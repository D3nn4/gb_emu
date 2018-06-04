#include "binaryinstructions.hpp"

void BinaryInstructions::checkBitInRegister(int bit, IMemory::REG8BIT reg, IMemory& memory)
{
    if (memory.isSet(bit, reg)){
        memory.unsetBitInRegister((int)IMemory::FLAG::Z, IMemory::REG8BIT::F);
    }
    else {
        memory.setBitInRegister((int)IMemory::FLAG::Z, IMemory::REG8BIT::F);
    }
}
int BinaryInstructions::checkBit7InH(IMemory& memory)
{
    checkBitInRegister(7, IMemory::REG8BIT::H, memory);
    return 0;
}
