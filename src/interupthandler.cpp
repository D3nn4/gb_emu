#include "interupthandler.hpp"

InteruptHandler::InteruptHandler(IMemory& memory)
    :_memory(memory),
     _masterInteruptSwitch(false){};

bool InteruptHandler::isInteruptEnable()
{
    return _masterInteruptSwitch;
}
