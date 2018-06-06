#ifndef _IINSTRUCTIONS_
#define _IINSTRUCTIONS_

#include "imemory.hpp"

class IInstructions
{
public:
  IInstructions(int cycles)
    :_cycles(cycles){};

  int doOp(IMemory& memory) {
    doInstruction(memory);
    return _cycles;
  }

  virtual void doInstruction(IMemory& memory) = 0;

  int _cycles;
};
#endif /*IINSTRUCTIONS*/
