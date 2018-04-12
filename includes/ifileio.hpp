#ifndef _IFILEIO_
#define _IFILEIO_

#include <string>
#include <vector>
#include <cstdint>

class IFileIO
{
public:
    virtual int openFile(std::string const & fileName) = 0;
    virtual int readFile(uint8_t *buff, int fd) = 0;
    virtual void closeFile(int fd) = 0;
};
#endif /*IFILEIO*/
