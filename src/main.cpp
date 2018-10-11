#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "fileio.hpp"
#include "romloader.hpp"
#include "cpu.hpp"

int main()
{
    std::cout << "welcome in gb_emu.\n";
    std::string fileName = "./roms/Pokemon_Bleue.gb";
    FileIO fileIO;
    RomLoader romLoader(fileIO);
    Cpu cpu(romLoader);
    // cpu.launchGame(fileName);
    return 0;
}
