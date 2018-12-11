#include "instructionhandler.hpp"

InstructionHandler::InstructionHandler(IMemory& memory, IInterruptHandler& interruptHandler)
    :_memory(memory),
     _interruptHandler(interruptHandler){};
     // _bootRom(BootRom()){};

int InstructionHandler::doInstruction(uint8_t opCode)
{
    auto instructMapIt = _instructions.find(opCode);
    if (instructMapIt == _instructions.end()) {
        throw InstructionException(__PRETTY_FUNCTION__);
    }
    std::shared_ptr<IInstructions> instruction = instructMapIt->second;
    return instruction->doOp(_memory);
}
// bool InstructionHandler::boot()
// {
//     // uint16_t& PC = _memory._registers.pc;
//     // while (PC < 0x0100) {
//     //     auto itInstruction = _instructions.find(_bootRom._bootDMG[PC]);
//     //     if (itInstruction != _instructions.end()) {
//     //         PC = itInstruction->second(PC);
//     //     }
//     //     else {
//     //         std::cout << PC << " not found\n";
//     //         PC = 0x0100;
//     //     }
//     // }
//     // PC = 0x0000;
//     return true;
// }


