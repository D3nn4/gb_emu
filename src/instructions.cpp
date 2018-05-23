#include <iostream>
#include "instructions.hpp"
uint16_t Instructions::combine8BitTo16Bit(uint8_t lhs, uint8_t rhs)
{
    return ((uint16_t)lhs << 8) | rhs;
}

void Instructions::load16NextBitToRegister(IMemory::REG16BIT reg, IMemory& memory)
{
    uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
    IMemory::RomData const & rom = memory.getReadOnlyMemory();
    uint16_t value = combine8BitTo16Bit(rom[cursor + 2],rom[cursor + 1]);
    memory.set16BitRegister(reg, value);
    memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 3);
}

void Instructions::load8BitInRegisterAtAdress(IMemory::REG8BIT reg, IMemory::REG16BIT adress, IMemory& memory)
{
    uint8_t valueRegister = memory.get8BitRegister(reg);
    uint16_t valueAdress = memory.get16BitRegister(adress);
    memory.writeInROM(valueRegister, valueAdress);
    uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
    memory.set16BitRegister(IMemory::REG16BIT::PC, ++cursor);
}

void Instructions::xor8BitRegister(IMemory::REG8BIT reg, IMemory& memory)
{
    uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
    memory.set8BitRegister(reg, 0);
    memory.set16BitRegister(IMemory::REG16BIT::PC, ++cursor);
}

int Instructions::load16BitToHL(IMemory& memory)
{
    load16NextBitToRegister(IMemory::REG16BIT::HL, memory);
    return 8; //Cycles
}

int Instructions::load16BitToSP(IMemory& memory)
{
    load16NextBitToRegister(IMemory::REG16BIT::SP, memory);
    return 8; //Cycles
}

int Instructions::loadAToAdressInHLAndDecrement(IMemory& memory)
{
    load8BitInRegisterAtAdress(IMemory::REG8BIT::A, IMemory::REG16BIT::HL, memory);
    uint16_t hLValue = memory.get16BitRegister(IMemory::REG16BIT::HL);
    hLValue--;
    memory.set16BitRegister(IMemory::REG16BIT::HL, hLValue);
    return 8;//Cycles
}

int Instructions::xorRegisterA(IMemory& memory)
{
    xor8BitRegister(IMemory::REG8BIT::A, memory);
    return 4; //Cycles
}

int Instructions::doBinaryInstructions(IMemory& memory)
{
    uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
    cursor++;
    //TODO : call binary instruction bootrom[cursor]
    memory.set16BitRegister(IMemory::REG16BIT::PC, ++cursor);
    return 0;//TODO find cycle to return
}
