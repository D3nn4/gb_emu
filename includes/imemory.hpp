#ifndef _IMEMORY_
#define _IMEMORY_

#include <array>
#include <map>
#include "registers.hpp"

class IMemory
{
public:
    static size_t const cartridgeSize =  0x200000;
    static size_t const romSize =  0x10000;
    static size_t const bank0Size =  0x4000;
    static size_t const readOnlyBankSize =  0x8000;

    using CartridgeData = std::array<uint8_t, cartridgeSize>;
    using RomData = std::array<uint8_t, romSize>;

    struct State
    {
        uint8_t opCode;
        uint16_t pcValue;
        std::map<std::string, uint16_t> reg16Bit;
        std::map<std::string, bool> flags;
        uint8_t IE;
        uint8_t IF;
        RomData readOnlyMemory;
    };
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

    virtual void incrementDivderRegister() = 0;

    virtual CartridgeData const getCartridge() = 0;
    virtual RomData const getReadOnlyMemory() = 0;
    virtual bool setCartridge(CartridgeData const & cartridge) = 0;
    virtual State getState() = 0;
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

static std::map<IMemory::FLAG, std::string> debugflag = {
    {IMemory::FLAG::Z, "z"},
    {IMemory::FLAG::N, "n"},
    {IMemory::FLAG::H, "h"},
    {IMemory::FLAG::C, "c"}
};
static std::map<IMemory::REG8BIT, std::string>  debugReg8Bit = {
    {IMemory::REG8BIT::A, "a"},
    {IMemory::REG8BIT::F, "f"},
    {IMemory::REG8BIT::B, "b"},
    {IMemory::REG8BIT::C, "c"},
    {IMemory::REG8BIT::D, "d"},
    {IMemory::REG8BIT::E, "e"},
    {IMemory::REG8BIT::H, "h"},
    {IMemory::REG8BIT::L, "l"}
};

static std::map<IMemory::REG16BIT, std::string>  debugReg16Bit = {
    {IMemory::REG16BIT::AF, "af"},
    {IMemory::REG16BIT::BC, "bc"},
    {IMemory::REG16BIT::DE, "de"},
    {IMemory::REG16BIT::HL, "hl"}
};
#endif /*IMEMORY*/
