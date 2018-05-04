#include "instructionhandler.hpp"

InstructionHandler::InstructionHandler(IMemory& memory)
    :_memory(memory){};

bool InstructionHandler::boot()
{
    // uint16_t& PC = _memory._registers.pc;
    // while (PC < 0x0100) {
    //     auto itInstruction = _instructions.find(_bootRom._bootDMG[PC]);
    //     if (itInstruction != _instructions.end()) {
    //         PC = itInstruction->second(PC);
    //     }
    //     else {
    //         std::cout << PC << " not found\n";
    //         PC = 0x0100;
    //     }
    // }
    // PC = 0x0000;
    return true;
}


