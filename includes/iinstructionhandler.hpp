#ifndef _IINSTRUCTIONHANDLER_
#define _IINSTRUCTIONHANDLER_

#include "imemory.hpp"

class IInstructionHandler
{
public:

    virtual bool boot() = 0;
};
#endif /*IINSTRUCTIONHANDLER*/
