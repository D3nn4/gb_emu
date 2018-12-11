#include <algorithm>
#include <iostream>
#include <bitset>
#include "memory.hpp"
#include "itimer.hpp"

Memory::Memory()
{
    reset();
}

void Memory::setTimer(ITimer* timer)
{
    _timer = timer;
}

void Memory::incrementDivderRegister()
{
    _readOnlyMemory[_timer->_DIV]++;
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
        initializeMemory();
        return true;
    }
    return false;
}

void Memory::initializeMemory()
{
    _registers.pc = 0x0100;
    _registers.sp = 0xfffe;
    _registers.af = 0x01b0;
    _registers.bc = 0x0013;
    _registers.de = 0x00d8;
    _registers.hl = 0x014d;

    _readOnlyMemory[0xFF05] = 0x00;
    _readOnlyMemory[0xFF06] = 0x00;
    _readOnlyMemory[0xFF07] = 0x00;
    _readOnlyMemory[0xFF10] = 0x80;
    _readOnlyMemory[0xFF11] = 0xBF;
    _readOnlyMemory[0xFF12] = 0xF3;
    _readOnlyMemory[0xFF14] = 0xBF;
    _readOnlyMemory[0xFF16] = 0x3F;
    _readOnlyMemory[0xFF17] = 0x00;
    _readOnlyMemory[0xFF19] = 0xBF;
    _readOnlyMemory[0xFF1A] = 0x7F;
    _readOnlyMemory[0xFF1B] = 0xFF;
    _readOnlyMemory[0xFF1C] = 0x9F;
    _readOnlyMemory[0xFF1E] = 0xBF;
    _readOnlyMemory[0xFF20] = 0xFF;
    _readOnlyMemory[0xFF21] = 0x00;
    _readOnlyMemory[0xFF22] = 0x00;
    _readOnlyMemory[0xFF23] = 0xBF;
    _readOnlyMemory[0xFF24] = 0x77;
    _readOnlyMemory[0xFF25] = 0xF3;
    _readOnlyMemory[0xFF26] = 0xF1;
    _readOnlyMemory[0xFF40] = 0x91;
    _readOnlyMemory[0xFF42] = 0x00;
    _readOnlyMemory[0xFF43] = 0x00;
    _readOnlyMemory[0xFF45] = 0x00;
    _readOnlyMemory[0xFF47] = 0xFC;
    _readOnlyMemory[0xFF48] = 0xFF;
    _readOnlyMemory[0xFF49] = 0xFF;
    _readOnlyMemory[0xFF4A] = 0x00;
    _readOnlyMemory[0xFF4B] = 0x00;
    _readOnlyMemory[0xFFFF] = 0x00;

}

bool Memory::writeInMemory(uint8_t data, uint16_t adress)
{
    //read only memory
    if (adress < 0x8000) {
        //TODO
        // writes a byte to memory. Remember that address 0 - 07FFF is rom so we cant write to this address
        // writing to memory address 0x0 to 0x1FFF this disables writing to the ram bank. 0 disables, 0xA enables
        // if writing to a memory address between 2000 and 3FFF then we need to change rom bank
        // writing to address 0x4000 to 0x5FFF switches ram banks (if enabled of course)
        // writing to address 0x6000 to 0x7FFF switches memory model
        return false;
    }
    else if (0xc000 <= adress && adress <= 0xdfff) {
        _readOnlyMemory[adress] = data;
    }
    //write to echo ram also write in ram
    else if (0xe000 <= adress && adress <= 0xfdff) {
        _readOnlyMemory[adress] = data;
        writeInMemory(data, adress - 0x2000);
    }
    //TODO restricted area
    else if (0xfea0 <= adress && adress <= 0xfeff){}
    else if (adress == _timer->_DIV) {
        _readOnlyMemory[_timer->_DIV] = 0;
    }
    else if (adress == _timer->_TMC) {
        int currentFrequency = _timer->getClockFrequency();
        _readOnlyMemory[_timer->_TMC] = data;
        int newFrequency = _timer->getClockFrequency();
        if(currentFrequency != newFrequency) {
            _timer->setClockFrequency();
        }
    }
    else if (adress == 0xff44) {
        //TODO FF44 shows which horizontal scanline is currently being draw. Writing here resets it
        _readOnlyMemory[0xff44] = 0;
    }
    else if (adress == 0xff46) {
        //TODO DMA transfer
    }
    else if (0xff4c <= adress && adress <= 0xff7f){}

    else {
        _readOnlyMemory[adress] = data;
    }
    return true;
}

uint8_t Memory::readInMemory(uint16_t adress)
{
    //TODO
    return _readOnlyMemory[adress];
}

bool Memory::fillROM()
{
    std::copy(_cartridge.begin(),
              _cartridge.begin() + IMemory::readOnlyBankSize,
              _readOnlyMemory.begin());
    return !isEmpty(_readOnlyMemory);
}

bool Memory::reset()
{
    _readOnlyMemory.fill(0x0);
    _cartridge.fill(0x0);
    _registers.pc = 0x0000;
    _registers.sp = 0x0000;
    _registers.af = 0x0000;
    _registers.bc = 0x0000;
    _registers.de = 0x0000;
    _registers.hl = 0x0000;
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

void Memory::setFlag(IMemory::FLAG flag)
{
    uint8_t regValue = *_8BitRegisters[IMemory::REG8BIT::F];
    std::bitset<8> bitsetFlag(regValue);
    bitsetFlag.set(static_cast<int>(flag));
    // regValue = 1 << static_cast<int>(flag);
    set8BitRegister(IMemory::REG8BIT::F, static_cast<uint8_t>(bitsetFlag.to_ulong()));
}

void Memory::unsetFlag(IMemory::FLAG flag)
{
    uint8_t regValue = *_8BitRegisters[IMemory::REG8BIT::F];
    std::bitset<8> bitsetFlag(regValue);
    bitsetFlag.reset(static_cast<int>(flag));
    set8BitRegister(IMemory::REG8BIT::F, static_cast<uint8_t>(bitsetFlag.to_ulong()));
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

void Memory::setBitInRegister(int bit, REG8BIT reg)
{
    if (bit > 7) {
        throw MemoryException(__PRETTY_FUNCTION__);
    }
    uint8_t regValue = *_8BitRegisters[reg];
    regValue = 1 << bit;
    set8BitRegister(reg, regValue);
}

bool Memory::isSetFlag(FLAG flag)
{
    int flagValue = static_cast<int>(flag);
    if (flagValue > 7) {
        throw MemoryException(__PRETTY_FUNCTION__);
    }
    uint8_t regValue = *_8BitRegisters[REG8BIT::F];
    std::bitset<8> bitset(regValue);
    return bitset.test(flagValue);
}

bool Memory::isSet(int bit, REG8BIT reg)
{
    if (bit > 7) {
        throw MemoryException(__PRETTY_FUNCTION__);
    }
    uint16_t regValue = *_8BitRegisters[reg];
    std::bitset<8> bitset(regValue);
    return bitset.test(bit);

}
