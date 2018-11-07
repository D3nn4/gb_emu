#ifndef _INTERRUPTHANDLER_
#define _INTERRUPTHANDLER_

#include "imemory.hpp"
#include "iinterupthandler.hpp"

class InterruptHandler : public IInterruptHandler
{
public:

    InterruptHandler(IMemory& memory);
    void doInterrupt() override;
    bool isMasterSwitchEnabled() override;
    void enableMasterSwitch() override;
    void disableMasterSwitch() override;
    void requestInterrupt(int id) override;
    // void halt();
    // void stop();


private:
    void serviceInterrupt(int id, std::bitset<8> bitsetRequest);

    IMemory& _memory;
};
#endif /*INTERRUPTHANDLER*/
