#ifndef _IINSTRUCTIONHANDLER_
#define _IINSTRUCTIONHANDLER_

#include "imemory.hpp"

class IInstructionHandler
{
public:


    virtual int doInstruction(uint8_t opCode) = 0;
};
#endif /*IINSTRUCTIONHANDLER*/
