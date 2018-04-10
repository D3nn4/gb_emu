#include <iostream>
#include "romloader.hpp"

RomLoader::RomLoader(IFileIO& fio)
    :fileIO(fio){}

bool RomLoader::load(std::string romName)
{
    int fd = fileIO.openFile(romName);
    if (fd > 0) {
        uint8_t buff[512];
        while (int readBit = fileIO.readFile(buff, fd)) {
            for (int i = 0; i < readBit; i++) {
                _data.push_back(buff[i]);
            }
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
