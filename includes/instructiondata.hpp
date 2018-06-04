#ifndef _INSTRUCTIONDATA_
#define _INSTRUCTIONDATA_

#include <functional>

typedef std::function<int(IMemory&)> funcInstruct;

struct InstructionData {
    int cycle;
    funcInstruct  handler;
};

#endif /*INSTRUCTIONDATA*/
