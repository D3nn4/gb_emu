#ifndef _MEMORY_
#define _MEMORY_

#include "imemory.hpp"

class Memory : public IMemory
{
public:
    std::vector<uint8_t> const & getCartridge();
    std::vector<uint8_t> const & getReadOnlyMemory();
    bool setCartridge(std::vector<uint8_t> cartridge);
private:

    bool reset();
    bool fillROM();
    std::vector<uint8_t> _cartridge;
    std::vector<uint8_t> _readOnlyMemory;
    //TODO MemoryBankController
};
#endif /*MEMORY*/
