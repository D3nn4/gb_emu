#include <iostream>
#include "cpu.hpp"
#include <cstring>

Cpu::Cpu(IRomLoader& romLoader)
    :_romLoader(romLoader),
     _interruptHandler(_memory),
     _timer(_memory, _interruptHandler),
     _instructionHandler(_memory, _interruptHandler),
     _graphics(_memory, _interruptHandler)
     // _renderer(_graphics.getScreenData())
{
    _memory.setTimer(&_timer);
}

int Cpu::getCurrentCycles()
{
    return _cycles;
}

IMemory::State Cpu::getState()
{
    return _memory.getState();
}


void Cpu::nextStep()
{
    //For debug
    // _gameLoaded = false;
    //For debug
    uint16_t pcValue = _memory.get16BitRegister(IMemory::REG16BIT::PC);
    uint8_t opCode = _memory.readInMemory(pcValue);
    _readableInstructionStream.str({}); // reset
    _readableInstructionStream <<  "[" << std::hex << 
        static_cast<int>(opCode) << "] ";
    BOOST_LOG_TRIVIAL(debug) << "[" << std::hex << static_cast<int>(opCode) << "]";
    if (opCode == 0x10 || opCode == 0x76) {
        _gameLoaded = false;
        return ;
    }
    try {
        int cycles = _instructionHandler.doInstruction(opCode);
        _cycles += cycles;
        _timer.update(cycles);
        //TODO
        _graphics.update(cycles);
        _interruptHandler.doInterrupt();
    }
    catch (...) {
        std::cout << "error catch\n";
    }
}

std::string Cpu::getReadableInstruction()
{
    // _readableInstructionStream <<  "[" << std::hex << 
    //     static_cast<int>(memory.getCurrentOpCode()) << "] ";
    _readableInstructionStream << _instructionHandler.getReadableInstruction();
    return _readableInstructionStream.str();
}

void Cpu::updateDebug()
{
    if (_gameLoaded) {
        if (_cycles < _maxCycles) {
            nextStep();
        }
        if (!(_cycles < _maxCycles)) {
            _cycles -= _maxCycles;
        }
    }
}

void Cpu::update()
{
    while (_gameLoaded) {
        while (_gameLoaded &&_cycles < _maxCycles) {
            nextStep();
        }
        //TODO
        //render sfml
        auto screen = _graphics.getScreenData();
        for (int y = 0; y < 144; y++) {
            for (int x = 0; x < 160; x++) {
                _screenBuffer[y][x][0] =  screen[y][x]._red;
                _screenBuffer[y][x][1] =  screen[y][x]._green;
                _screenBuffer[y][x][2] =  screen[y][x]._blue;
                _screenBuffer[y][x][3] = 1;
            }
        }
        emit screen_refresh();
        _cycles -= _maxCycles;
    }
}

bool Cpu::launchGameDebug(std::string const & cartridgeName)
{
    if (_romLoader.load(cartridgeName)
        && _memory.setCartridge(_romLoader.getData())) {
        _gameLoaded = true;
        return true;
    }
    return false;
}

bool Cpu::launchGame(std::string const & cartridgeName)
{
    if (_romLoader.load(cartridgeName)
        && _memory.setCartridge(_romLoader.getData())) {
            _gameLoaded = true;
            update();
    }
    else {
        std::cout << "error loading cartridge\n";
        return false;
    }
    return true;
}

void Cpu::stopGame()
{
    _gameLoaded = false;
}

