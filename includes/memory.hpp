#ifndef _MEMORY_
#define _MEMORY_

#include "imemory.hpp"
#include "registers.hpp"

class Memory : public IMemory
{
public:

    CartridgeData const & getCartridge();
    RomData const & getReadOnlyMemory();
    bool setCartridge(CartridgeData const & cartridge);

private:

    bool reset();
    bool fillROM();
  template <class ARRAY>
  bool isEmpty(ARRAY const & memory);

    CartridgeData _cartridge;
    RomData _readOnlyMemory;
    Registers _registers;
    //TODO MemoryBankController
};
#endif /*MEMORY*/
