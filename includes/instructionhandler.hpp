#ifndef _INSTRUCTIONHANDLER_
#define _INSTRUCTIONHANDLER_

#include <string>
#include <vector>
#include "iinstructionhandler.hpp"
#include "bootrom.hpp"
#include "imemory.hpp"

class InstructionHandler : public IInstructionHandler
{
public:

    InstructionHandler(IMemory& memory);
    bool boot() override;

private:

    IMemory& _memory;
    BootRom _bootRom;
};
#endif /*INSTRUCTIONHANDLER*/
