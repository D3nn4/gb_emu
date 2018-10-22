#ifndef _IMEMORY_
#define _IMEMORY_

#include <array>
#include "registers.hpp"

class IMemory
{
public:
    static size_t const cartridgeSize =  0x200000;
    static size_t const romSize =  0x10000;
    static size_t const bank0Size =  0x4000;

    using CartridgeData = std::array<uint8_t, cartridgeSize>;
    using RomData = std::array<uint8_t, romSize>;

    enum class REG8BIT
        {
            A, F,
            B, C,
            D, E,
            H, L
        };

    enum class REG16BIT
        {
            AF,
            BC,
            DE,
            HL,
            SP,
            PC
        };

    enum class FLAG
        {
            Z = 7,
            N = 6,
            H = 5,
            C = 4
        };

    virtual CartridgeData const getCartridge() = 0;
    virtual RomData const getReadOnlyMemory() = 0;
    virtual bool setCartridge(CartridgeData const & cartridge) = 0;
    virtual bool writeInMemory(uint8_t data, uint16_t adress) = 0;
    virtual uint8_t readInMemory(uint16_t adress) = 0;
    virtual void set8BitRegister(REG8BIT reg,uint8_t value) = 0;
    virtual void set16BitRegister(REG16BIT reg,uint16_t value) = 0;
    virtual uint8_t get8BitRegister(REG8BIT reg) = 0;
    virtual uint16_t get16BitRegister(REG16BIT reg) = 0;
    virtual void setFlag(IMemory::FLAG flag) = 0;
    virtual void unsetFlag(IMemory::FLAG flag) = 0;
    virtual bool isSetFlag(IMemory::FLAG flag) = 0;
    virtual void setBitInRegister(int bit, REG8BIT reg) = 0;
    virtual void unsetBitInRegister(int bit, REG8BIT reg) = 0;
    virtual bool isSet(int bit, REG8BIT reg) = 0;

};
#endif /*IMEMORY*/
