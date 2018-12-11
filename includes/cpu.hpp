#ifndef _CPU_
#define _CPU_

#include <string>
#include "memory.hpp"
#include "iromloader.hpp"
#include "instructionhandler.hpp"
#include "interupthandler.hpp"
#include "timer.hpp"

class Cpu
{
public:
    Cpu(IRomLoader& romloader);
    int getCurrentCycles();
    // void boot();
    void update();
    void launchGame(std::string const & cartridgeName);
    void stopGame();

private:

    bool _gameLoaded = false;
    int _cycles = 0;
    int const _maxCycles = 70221;
    Memory _memory;
    IRomLoader& _romLoader;
    InterruptHandler _interruptHandler;
    Timer _timer;
    InstructionHandler _instructionHandler;



};
#endif /*CPU*/
