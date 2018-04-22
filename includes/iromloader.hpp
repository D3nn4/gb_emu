#ifndef _IROMLOADER_
#define _IROMLOADER_

#include <string>
#include "fileio.hpp"

class IRomLoader
{
public:
    virtual bool load(std::string const & romName) = 0;
    virtual std::vector<uint8_t> getData() = 0;

};
#endif /*IROMLOADER*/
