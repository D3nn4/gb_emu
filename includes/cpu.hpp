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
    int getCurrentCycles();
    // void boot();
    void executeOneFrame();
    void launchGame(std::string const & cartridgeName);

private:


    int _cycles = 0;
    int const _maxCycles = 70244;
    Memory _memory;
    IRomLoader& _romLoader;
    InstructionHandler _instructionHandler;



};
#endif /*CPU*/
