#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "fileio.hpp"

int FileIO::openFile(std::string const & fileName)
{
    int fd = open(fileName.c_str(), O_RDONLY);
    if (fd < 0) {
        std::string error = strerror(errno);
        //TODO error handler
        std::cout << "error open : " << error << std::endl;
    }
    return fd;
}


int FileIO::readFile(uint8_t *buff, int fd)
{
    return read(fd, buff, 512);
}

void FileIO::closeFile(int fd)
{
    close(fd);
}
