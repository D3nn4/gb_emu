#include <bitset>
#include <iostream>

#include "timer.hpp"


Timer::Timer(IMemory& memory, IInterruptHandler& interruptHandler)
    :_memory(memory),
     _interruptHandler(interruptHandler){};


bool Timer::isOn()
{
    uint8_t timerControler = _memory.readInMemory(_TMC);
    std::bitset<8> timerControlerBitset (timerControler);
    return timerControlerBitset.test(2);
}

void Timer::update(int cycles)
{
    doDividerRegister(cycles);

    if(isOn()) {
        _cycleCounter -= cycles;

        if (_cycleCounter <= 0) {
            setClockFrequency();

            uint8_t timerCounter = _memory.readInMemory(_TIMA);
            if (timerCounter == 0xff) {
                uint8_t timerModulo = _memory.readInMemory(_TMA);
                _memory.writeInMemory(timerModulo, _TIMA);
                _interruptHandler.requestInterrupt(IInterruptHandler::INTERRUPT::TIMER);
            }
            else {
                _memory.writeInMemory(timerCounter + 1, _TIMA);
            }
        }
    }
}

uint32_t Timer::getClockFrequency()
{
    uint8_t tmc = _memory.readInMemory(_TMC) & 0x03;
    auto const & frequency = _speed.find(tmc);
    return frequency->second;
}

int Timer::setClockFrequency()
{
    uint32_t frequency = getClockFrequency();
    _cycleCounter = _clockSpeed/frequency;
    return _cycleCounter;
}

void Timer::doDividerRegister(int cycles)
{
    _dividerRegister += cycles;
    if(_dividerRegister >= 0xff) {
        _dividerRegister = 0;
        _memory.incrementDivderRegister();
    }
}
