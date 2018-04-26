#ifndef _ROMLOADER_
#define _ROMLOADER_

#include <string>
#include <vector>
#include "fileio.hpp"
#include "iromloader.hpp"

class RomLoader : public IRomLoader
{
public:
    RomLoader(IFileIO& fio);
    bool load(std::string const & romName);
    IMemory::CartridgeData getData();
private:
    IFileIO& fileIO;
    IMemory::CartridgeData _data;

};
#endif /*ROMLOADER*/
