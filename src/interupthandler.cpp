#include <bitset>
#include <iostream>
#include "interupthandler.hpp"

InterruptHandler::InterruptHandler(IMemory& memory)
    :_memory(memory){};


void InterruptHandler::doInterrupt()
{
    if (isMasterSwitchEnabled()) {
        uint8_t interruptRequest = _memory.readInMemory(_interruptRequestRegister);
        uint8_t interruptEnabled = _memory.readInMemory(_interruptEnableRegister);

        std::bitset<8> bitsetRequest(interruptRequest);
        std::bitset<8> bitsetEnabled(interruptEnabled);

        if (interruptRequest) {
            for (int interruptID = 0; interruptID < 5; interruptID++) {
                if (bitsetRequest.test(interruptID)
                    && bitsetEnabled.test(interruptID)) {
                    serviceInterrupt(interruptID, bitsetRequest);
                }
            }
        }
    }
}

bool InterruptHandler::isMasterSwitchEnabled()
{
    return _masterInterruptSwitch;
}

void InterruptHandler::enableMasterSwitch()
{
    _masterInterruptSwitch = true;
}

void InterruptHandler::disableMasterSwitch()
{
    _masterInterruptSwitch = false;
}

void InterruptHandler::requestInterrupt(int id)
{
    uint8_t interruptRequest = _memory.readInMemory(_interruptRequestRegister);
    std::bitset<8> bitsetRequest(interruptRequest);
    bitsetRequest.set(id);
    _memory.writeInMemory(bitsetRequest.to_ulong(), _interruptRequestRegister);
}

void InterruptHandler::serviceInterrupt(int id, std::bitset<8> bitsetRequest)
{
   _masterInterruptSwitch = false;
   bitsetRequest.reset(id);
   _memory.writeInMemory(bitsetRequest.to_ulong(), _interruptRequestRegister);

   uint16_t programCounter = _memory.get16BitRegister(IMemory::REG16BIT::PC);
   uint16_t stackPointer = _memory.get16BitRegister(IMemory::REG16BIT::SP);
   uint8_t mostSignificantBit = static_cast<uint8_t>((programCounter >> 8) & 0xff);
   uint8_t lessSignificantBit = static_cast<uint8_t>(programCounter & 0xff);
   _memory.writeInMemory(lessSignificantBit, stackPointer - 1);
   _memory.writeInMemory(mostSignificantBit, stackPointer);

   _memory.set16BitRegister(IMemory::REG16BIT::SP, stackPointer - 2);
   _memory.set16BitRegister(IMemory::REG16BIT::PC, serviceRoutineAdress[id]);
}
