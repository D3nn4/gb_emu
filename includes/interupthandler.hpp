#ifndef _INTERUPTHANDLER_
#define _INTERUPTHANDLER_

#include "imemory.hpp"
#include "iinterupthandler.hpp"

class InteruptHandler
{
public:
    InteruptHandler(IMemory& memory);
    // void doInterupt() override;
    // void requestInterupt(int id) override;

    bool isInteruptEnable();

private:
    IMemory& _memory;
    bool _masterInteruptSwitch;
};
#endif /*INTERUPTHANDLER*/
