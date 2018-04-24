#ifndef _ROMLOADER_
#define _ROMLOADER_

#include <string>
#include <vector>
#include "fileio.hpp"

class RomLoader
{
public:
    RomLoader(IFileIO& fio);
    bool load(std::string const & romName);
    std::vector<uint8_t> getData();
private:
    IFileIO& fileIO;
    std::vector<uint8_t> _data;

};
#endif /*ROMLOADER*/
