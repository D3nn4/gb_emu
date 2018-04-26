#ifndef _IMEMORY_
#define _IMEMORY_

#include <vector>
#include <array>

class IMemory
{
public:
    static size_t const cartridgeSize =  0x200000;
    static size_t const romSize =  0x10000;
    static size_t const bank0Size =  0x4000;

    using CartridgeData = std::array<uint8_t, cartridgeSize>;
    using RomData = std::array<uint8_t, romSize>;

    virtual CartridgeData const & getCartridge() = 0;
    virtual RomData const & getReadOnlyMemory() = 0;
    virtual bool setCartridge(CartridgeData const & cartridge) = 0;
};
#endif /*IMEMORY*/
