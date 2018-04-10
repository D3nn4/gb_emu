#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "fileio.hpp"
#include "romloader.hpp"
int main()
{
    std::cout << "welcome in gb_emu.\n";
    std::string fileName = "./roms/Pokemon_Bleue.gb";
    FileIO fileIO;
    RomLoader romLoader(fileIO);
    
    // int fd = open("./roms/Pokemon_Bleue.gb", O_RDONLY);
    // std::vector<uint8_t> memory;
    // if (fd >= 0) {
    //   uint8_t buff[512];
    //   while (int ret = read(fd, buff, 512)) {
    //     for (int i = 0; i < ret; i++) {
    //       memory.push_back(buff[i]);
    //     }
    //   }
    //   size_t nameBegin = 0x100;
    //   size_t nameEnd = 0x14F;
    //   std::string name;
    //   int fdOutput = open("name", O_WRONLY | O_CREAT, 0666);
    //   if (fdOutput >= 0) {
    //     std::string name(memory.begin() + nameBegin, memory.begin() + nameEnd);
    //     std::cout << name << std::endl;
    //   }
    // }
    // else {
    //   std::string error = strerror(errno);
    //   std::cout << "error open : " << error << std::endl;
    // }
    return 0;
}
