#ifndef _MEMORY_
#define _MEMORY_

#include "imemory.hpp"
#include "registers.hpp"

class Memory : public IMemory
{
public:

    IMemory::CartridgeData const & getCartridge();
    RomData const & getReadOnlyMemory();
    bool setCartridge(IMemory::CartridgeData const & cartridge);

private:

    bool reset();
    bool fillROM();
  template <class ARRAY>
  bool isEmpty(ARRAY const & memory);

    IMemory::CartridgeData _cartridge;
    RomData _readOnlyMemory;
    Registers _registers;
    //TODO MemoryBankController
};
#endif /*MEMORY*/
