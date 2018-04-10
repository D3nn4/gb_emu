#ifndef _FILEIO_
#define _FILEIO_

#include "ifileio.hpp"
class FileIO : public IFileIO
{
public:

    int openFile(std::string fileName);
    int readFile(uint8_t *buff, int fd);
    void closeFile(int fd);
};
#endif /*FILEIO*/
