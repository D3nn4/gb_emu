#include <iostream>
#include <algorithm>
#include "romloader.hpp"
#include <cstdio>

RomLoader::RomLoader(IFileIO& fio)
    :_fileIO(fio)
{
    _data.fill(0);
}

bool RomLoader::load(std::string const & romName)
{
    int fd = _fileIO.openFile(romName);
    if (fd > 0) {
        _data.fill(0);
        uint8_t buff[512];
        int readCount = 0x0100;
        while (int readBit = _fileIO.readFile(buff, fd)) {
            std::copy(std::begin(buff), std::end(buff), _data.begin() + readCount);
            readCount += readBit;
        }
        _fileIO.closeFile(fd);
    }
    return !std::all_of(std::begin(_data), std::end(_data),
                        []( uint8_t const & elem)
                        { return elem == 0; }
                        );
}

IMemory::CartridgeData RomLoader::getData()
{
    return _data;
}
