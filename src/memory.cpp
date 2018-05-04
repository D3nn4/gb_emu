#include <algorithm>
#include <iostream>
#include "memory.hpp"


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
    return true;
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
