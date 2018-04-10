#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "fileio.hpp"

int FileIO::openFile(std::string fileName)
{
    int fd = open("./roms/Pokemon_Bleue.gb", O_RDONLY);
    if (fd < 0) {
        std::string error = strerror(errno);
        std::cout << "error open : " << error << std::endl;
    }
    return fd;
}


int FileIO::readFile(uint8_t *buff, int fd)
{
    int ret = read(fd, buff, 512);
    return ret;
}

void FileIO::closeFile(int fd)
{
    close(fd);
}
