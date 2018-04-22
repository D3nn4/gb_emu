#ifndef _IMEMORY_
#define _IMEMORY_

#include <vector>
#include "romloader.hpp"

class IMemory
{
public:
    virtual std::vector<uint8_t> const & getCartridge() = 0;
    virtual std::vector<uint8_t> const & getReadOnlyMemory() = 0;
    virtual bool setCartridge(std::vector<uint8_t> cartridge) = 0;
};
#endif /*IMEMORY*/
