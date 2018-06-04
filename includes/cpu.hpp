#ifndef _CPU_
#define _CPU_

#include <string>
#include "memory.hpp"
#include "iromloader.hpp"
#include "instructionhandler.hpp"

class Cpu
{
public:
    Cpu(IRomLoader& romloader);
    void launchGame(std::string const & cartridgeName);

private:

    Memory _memory;
    IRomLoader& _romLoader;
    InstructionHandler _instructionHandler;



};
#endif /*CPU*/
