#include <iostream>
#include "cpu.hpp"
#include <cstring>

Cpu::Cpu(IRomLoader& romLoader)
    :_romLoader(romLoader),
     _interruptHandler(_memory),
     _timer(_memory, _interruptHandler),
     _instructionHandler(_memory, _interruptHandler)
{
    _memory.setTimer(&_timer);
}

int Cpu::getCurrentCycles()
{
    return _cycles;
}

void Cpu::update()
{
    while (_gameLoaded) {
        while (_cycles < _maxCycles) {
            //For debug
            // _gameLoaded = false;
            //For debug
            uint16_t pcValue = _memory.get16BitRegister(IMemory::REG16BIT::PC);
            uint8_t opCode = _memory.readInMemory(pcValue);
            BOOST_LOG_TRIVIAL(debug) << "[" << std::hex << static_cast<int>(opCode) << "]";
            if (opCode == 0x10 || opCode == 0x76) {
                _gameLoaded = false;
                break;
            }
            try {
                int cycles = _instructionHandler.doInstruction(opCode);
                _cycles += cycles;
                _timer.update(cycles);
                _interruptHandler.doInterrupt();
            }
            catch (...) {
                std::cout << "error catch\n";
            }
        }
        //TODO
        //render
        _cycles -= _maxCycles;
    }
}

void Cpu::launchGame(std::string const & cartridgeName)
{
    if (_romLoader.load(cartridgeName)
        && _memory.setCartridge(_romLoader.getData())) {
            _gameLoaded = true;
            update();
    }
    else {
        std::cout << "error loading cartridge\n";
    }
}

void Cpu::stopGame()
{
    _gameLoaded = false;
}

