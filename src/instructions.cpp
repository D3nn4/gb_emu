#include <iostream>
#include "instructions.hpp"

void Instructions::load16BitToSP(IMemory& memory)
{
    uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
    IMemory::RomData const & rom = memory.getReadOnlyMemory();
    uint8_t leastSignificantBit = rom[++cursor];
    uint8_t mostSignificantBit = rom[++cursor];
    uint16_t value = ((uint16_t)mostSignificantBit << 8) | leastSignificantBit;
    memory.set16BitRegister(IMemory::REG16BIT::SP, value);
    memory.set16BitRegister(IMemory::REG16BIT::PC, ++cursor);
}
