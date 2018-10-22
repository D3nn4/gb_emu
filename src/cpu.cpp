#include <iostream>
#include "cpu.hpp"
#include <cstring>

Cpu::Cpu(IRomLoader& romLoader)
    :_romLoader(romLoader),
     _instructionHandler(_memory){}

int Cpu::getCurrentCycles()
{
    return _cycles;
}
/*
void Cpu::executeOneFrame()
{
    while (_cycles < _maxCycles) {
        uint16_t pcValue = _memory.readInMemory(pcValue);
        uint8_t opCode = _memory.readInMemory(pcValue);
        try {
        	_cycles += _instructionHandler.doInstruction(opCode);
        }
        catch (...) {
        	std::cout << "error catch\n";
        }
        
    }
    _cycles -= _maxCycles;
}*/
// void Cpu::launchGame(std::string const & cartridgeName)
// {
//     if (_romLoader.load(cartridgeName)) {
//         _memory.setCartridge(_romLoader.getData());
//         _memory.set16BitRegister(IMemory::REG16BIT::PC, 0x0100);
//     }
//     else {
//         std::cout << "error loading cartridge\n";
//     }
// }

