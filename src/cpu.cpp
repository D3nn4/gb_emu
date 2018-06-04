#include <iostream>
#include "cpu.hpp"
#include <cstring>

Cpu::Cpu(IRomLoader& romLoader)
    :_romLoader(romLoader),
     _instructionHandler(_memory){}

void Cpu::launchGame(std::string const & cartridgeName)
{
    if (_romLoader.load(cartridgeName)) {
        _memory.setCartridge(_romLoader.getData());
        _memory.set16BitRegister(IMemory::REG16BIT::PC, 0x0000);
        _instructionHandler.boot();
    }
    else {
        std::cout << "error loading cartridge\n";
    }
}

