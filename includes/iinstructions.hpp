#ifndef _IINSTRUCTIONS_
#define _IINSTRUCTIONS_

#include "imemory.hpp"

class IInstructions
{
public:
    virtual void load16BitToSP(IMemory& memory) = 0;

private:

};
#endif /*IINSTRUCTIONS*/
