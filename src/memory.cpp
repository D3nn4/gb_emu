#include <algorithm>
#include <iostream>
#include "memory.hpp"

Memory::Memory()
{
    reset();
}

IMemory::CartridgeData const Memory::getCartridge()
{
    return _cartridge;
}

IMemory::RomData const Memory::getReadOnlyMemory()
{
    return _readOnlyMemory;
}

template<class ARRAY>
bool Memory::isEmpty(ARRAY const & memory)
{
    return std::all_of(std::begin(memory), std::end(memory),
                       []( typename ARRAY::value_type const & elem)
                       { return elem == 0; }
                       );
}

bool Memory::setCartridge(IMemory::CartridgeData const & cartridge)
{
    if (!isEmpty(cartridge) && reset()) {
        _cartridge = cartridge;
         fillROM();
         return true;
    }
    return false;
}

bool Memory::writeInROM(uint8_t data, uint16_t adress)
{
    //TODO
    if (adress < 0x8000) {
        return false;
    }
    _readOnlyMemory[adress] = data;
    return true;
}

bool Memory::fillROM()
{
    std::copy(_cartridge.begin(),
              _cartridge.begin() + IMemory::bank0Size,
              _readOnlyMemory.begin());
    return !isEmpty(_readOnlyMemory);
}

bool Memory::reset()
{
    //TODO check if everything is over
    _readOnlyMemory.fill(0x0);
    _cartridge.fill(0x0);
    resetRegisters();
    return true;
}

void Memory::resetRegisters()
{
    _registers.af = 0x0000;
    _registers.bc = 0x0000;
    _registers.de = 0x0000;
    _registers.hl = 0x0000;
    _registers.pc = 0x0000;
    _registers.sp = 0x0000;
}

void Memory::set8BitRegister(REG8BIT reg,uint8_t value)
{
    *_8BitRegisters[reg] = value;
}

void Memory::set16BitRegister(REG16BIT reg,uint16_t value)
{
    *_16BitRegisters[reg] = value;
}

uint8_t Memory::get8BitRegister(REG8BIT reg)
{
    return *_8BitRegisters[reg];
}

uint16_t Memory::get16BitRegister(REG16BIT reg)
{
    return *_16BitRegisters[reg];
}

void Memory::setBitInRegister(int bit, REG8BIT reg)
{
    if (bit > 7) {
        throw MemoryException(__PRETTY_FUNCTION__);
    }
    uint8_t regValue = *_8BitRegisters[reg];
    regValue = 1 << bit;
    set8BitRegister(reg, regValue);
}

void Memory::unsetBitInRegister(int bit, REG8BIT reg)
{
    if (bit > 7) {
        throw MemoryException(__PRETTY_FUNCTION__);
    }
    uint8_t regValue = *_8BitRegisters[reg];
    regValue = 0 << bit;
    set8BitRegister(reg, regValue);
}

void Memory::setBitInRegister(int bit, REG16BIT reg)
{
    if (bit > 15) {
        throw MemoryException(__PRETTY_FUNCTION__);
    }
    uint16_t regValue = *_16BitRegisters[reg];
    regValue = 1 << bit;
    set16BitRegister(reg, regValue);
}

void Memory::unsetBitInRegister(int bit, REG16BIT reg)
{
    if (bit > 15) {
        throw MemoryException(__PRETTY_FUNCTION__);
    }
    uint16_t regValue = *_16BitRegisters[reg];
    regValue = 0 << bit;
    set16BitRegister(reg, regValue);
}

bool Memory::isSet(int bit, REG8BIT reg)
{
    if (bit > 7) {
        throw MemoryException(__PRETTY_FUNCTION__);
    }
    uint8_t regValue = *_8BitRegisters[reg];
    std::bitset<8> bitset(regValue);
    return bitset.test(bit);
}

bool Memory::isSet(int bit, REG16BIT reg)
{
    if (bit > 15) {
        throw MemoryException(__PRETTY_FUNCTION__);
    }
    uint16_t regValue = *_16BitRegisters[reg];
    std::bitset<16> bitset(regValue);
    return bitset.test(bit);

}
