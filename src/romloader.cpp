#include <iostream>
#include "romloader.hpp"

RomLoader::RomLoader(IFileIO& fio)
    :fileIO(fio){}

bool RomLoader::load(std::string const & romName)
{
    int fd = fileIO.openFile(romName);
    if (fd > 0) {
        uint8_t buff[512];
        while (int readBit = fileIO.readFile(buff, fd)) {
            _data.insert(_data.end(), buff, buff + readBit);
        }
        fileIO.closeFile(fd);
        return true;
    }
    return false;
}

std::vector<uint8_t> RomLoader::getData()
{
    return _data;
}
