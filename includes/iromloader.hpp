#ifndef _IROMLOADER_
#define _IROMLOADER_

#include <string>
#include "fileio.hpp"
#include "imemory.hpp"

class IRomLoader
{
public:
    virtual bool load(std::string const & romName) = 0;
    virtual IMemory::CartridgeData getData() = 0;

};
#endif /*IROMLOADER*/
