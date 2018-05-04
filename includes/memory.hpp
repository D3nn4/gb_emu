#ifndef _MEMORY_
#define _MEMORY_

#include <map>
#include "imemory.hpp"

class Memory : public IMemory
{
public:

    CartridgeData const  getCartridge() override;
    RomData const  getReadOnlyMemory() override;
    bool setCartridge(CartridgeData const & cartridge) override;
    void set8BitRegister(REG8BIT reg,uint8_t value) override;
    void set16BitRegister(REG16BIT reg,uint16_t value) override;
    uint8_t get8BitRegister(REG8BIT reg) override;
    uint16_t get16BitRegister(REG16BIT reg) override;

private:

    bool reset();
    bool fillROM();
    template <class ARRAY>
    bool isEmpty(ARRAY const & memory);

    Registers _registers;
    CartridgeData _cartridge;
    RomData _readOnlyMemory;

    std::map<IMemory::REG8BIT, uint8_t*> _8BitRegisters =
        {
            {IMemory::REG8BIT::A, &_registers.a},
            {IMemory::REG8BIT::B, &_registers.b},
            {IMemory::REG8BIT::C, &_registers.c},
            {IMemory::REG8BIT::D, &_registers.d},
            {IMemory::REG8BIT::E, &_registers.e},
            {IMemory::REG8BIT::F, &_registers.f},
            {IMemory::REG8BIT::H, &_registers.h},
            {IMemory::REG8BIT::L, &_registers.l}
        };
    std::map<IMemory::REG16BIT, uint16_t*> _16BitRegisters = 
        {
            {IMemory::REG16BIT::AF, &_registers.pc},
            {IMemory::REG16BIT::BC, &_registers.pc},
            {IMemory::REG16BIT::DE, &_registers.pc},
            {IMemory::REG16BIT::HL, &_registers.pc},
            {IMemory::REG16BIT::PC, &_registers.pc},
            {IMemory::REG16BIT::SP, &_registers.pc}
        };
    //TODO MemoryBankController
};
#endif /*MEMORY*/
