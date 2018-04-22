#include <algorithm>
#include <iostream>
#include "memory.hpp"


std::vector<uint8_t> const & Memory::getCartridge()
{
    return _cartridge;
}

std::vector<uint8_t> const & Memory::getReadOnlyMemory()
{
    return _readOnlyMemory;
}

bool Memory::setCartridge(std::vector<uint8_t> cartridge)
{
    if (!cartridge.empty() && cartridge.size() >= 0x4000) {
        if (reset()) {
            _cartridge = cartridge;
            return fillROM();
        }
    }
    return false;
}

bool Memory::fillROM()
{
    _readOnlyMemory.clear();
    if (!_cartridge.empty()) {
        std::copy(_cartridge.begin(),
                  _cartridge.begin() + 0x3fff,
                  std::back_inserter(_readOnlyMemory));
        if (_readOnlyMemory.empty()) {
            return false;
        }
        return true;
    }
    return false;
}

bool Memory::reset()
{
    //TODO check if everything is over
    _readOnlyMemory.clear();
    _cartridge.clear();
    return true;
}
