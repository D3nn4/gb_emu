#ifndef _MEMORY_
#define _MEMORY_

#include <map>
#include <bitset>
#include <exception>
#include "imemory.hpp"
#include "itimer.hpp"


class Memory : public IMemory
{
public:

    class MemoryException : public std::exception
    {
    public:
        MemoryException(std::string const & error)
            :_error(error){}

        const char * what () const throw ()
        {
            return _error.c_str();
        }

    private:
        std::string _error;
    };

    Memory();
    void setTimer(ITimer* timer);
    void incrementDividerRegister() override;
    void incrementScanline() override;

    CartridgeData const  getCartridge() override;
    RomData const  getReadOnlyMemory() override;
    bool setCartridge(CartridgeData const & cartridge) override;
    State getState() override;
    bool writeInMemory(uint8_t data, uint16_t adress) override;
    uint8_t readInMemory(uint16_t adress) override;

    void set8BitRegister(REG8BIT reg,uint8_t value) override;
    void set16BitRegister(REG16BIT reg,uint16_t value) override;
    uint8_t get8BitRegister(REG8BIT reg) override;
    uint16_t get16BitRegister(REG16BIT reg) override;

    void setFlag(IMemory::FLAG flag) override;
    void unsetFlag(IMemory::FLAG flag) override;
    bool isSetFlag(IMemory::FLAG flag) override;

    void unsetBitInRegister(int bit, REG8BIT reg) override;
    void setBitInRegister(int bit, REG8BIT reg) override;
    bool isSet(int bit, REG8BIT reg) override;

private:

    bool reset();
    bool fillROM();
    void initializeMemory();
    template <class ARRAY>
    bool isEmpty(ARRAY const & memory);

    Registers _registers;
    CartridgeData _cartridge;
    RomData _readOnlyMemory;
    // unique_ptr<ITimer> _timer;
    ITimer* _timer;

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
            {IMemory::REG16BIT::AF, &_registers.af},
            {IMemory::REG16BIT::BC, &_registers.bc},
            {IMemory::REG16BIT::DE, &_registers.de},
            {IMemory::REG16BIT::HL, &_registers.hl},
            {IMemory::REG16BIT::PC, &_registers.pc},
            {IMemory::REG16BIT::SP, &_registers.sp}
        };
    //TODO MemoryBankController
};
#endif /*MEMORY*/
