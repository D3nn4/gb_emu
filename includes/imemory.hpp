#ifndef _IMEMORY_
#define _IMEMORY_

#include <vector>
#include "romloader.hpp"

class IMemory
{
public:
    virtual std::array<uint8_t, 0x200000> const & getCartridge() = 0;
    virtual std::array<uint8_t, 0xffff> const & getReadOnlyMemory() = 0;
    virtual bool setCartridge(std::array<uint8_t, 0x200000> const & cartridge) = 0;
};
#endif /*IMEMORY*/
