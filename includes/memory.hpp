#ifndef _MEMORY_
#define _MEMORY_

#include "imemory.hpp"
#include "registers.hpp"

class Memory : public IMemory
{
public:
    std::array<uint8_t, 0x200000> const & getCartridge();
    std::array<uint8_t, 0xffff> const & getReadOnlyMemory();
    bool setCartridge(std::array<uint8_t, 0x200000> const & cartridge);
private:

    bool reset();
    bool fillROM();
    std::array<uint8_t, 0x200000> _cartridge;
    std::array<uint8_t, 0xffff> _readOnlyMemory;
    Registers _registers;
    //TODO MemoryBankController
};
#endif /*MEMORY*/
