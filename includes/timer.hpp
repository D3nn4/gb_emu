#ifndef _TIMER_
#define _TIMER_

#include "itimer.hpp"
#include "imemory.hpp"
#include "iinterupthandler.hpp"

class Timer : public ITimer
{
public:

    Timer(IMemory& memory, IInterruptHandler& interruptHandler);

    // void turnOn() override;
    // void turnOff() override;
    void update(int cycles) override;
    bool isOn() override;
    uint32_t getClockFrequency() override;
    int setClockFrequency() override;

private:

    void doDividerRegister(int cycles);
    IMemory& _memory;
    IInterruptHandler& _interruptHandler;
};
#endif /*TIMER*/
