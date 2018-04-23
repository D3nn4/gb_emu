#include <algorithm>
#include <iostream>
#include "memory.hpp"


std::array<uint8_t, 0x200000> const & Memory::getCartridge()
{
    return _cartridge;
}

std::array<uint8_t, 0xffff> const & Memory::getReadOnlyMemory()
{
    return _readOnlyMemory;
}

bool isEmpty(std::array<uint8_t, 0x200000> const & cartridge)
{
    for (size_t index = 0; index < cartridge.size(); index++) {
        if (cartridge[index] != 0x0) {
            return false;
        }
    }
    return true;
}
bool isEmpty(std::array<uint8_t, 0xffff> const & rom)
{
    for (size_t index = 0; index < rom.size(); index++) {
        if (rom[index] != 0x0) {
            return false;
        }
    }
    return true;
}

bool Memory::setCartridge(std::array<uint8_t, 0x200000> const & cartridge)
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
              _cartridge.begin() + 0x3fff,
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
