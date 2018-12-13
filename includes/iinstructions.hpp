#ifndef _IINSTRUCTIONS_
#define _IINSTRUCTIONS_

#include <sstream>
#include <boost/log/trivial.hpp>
#include "imemory.hpp"

class IInstructions
{
public:
    IInstructions(int cycles)
        :_cycles(cycles){};

    int doOp(IMemory& memory) {
        doInstruction(memory);
        return _cycles;
    };


    std::string getReadableInstruction()
    {
        return _readableInstructionStream.str();
    }

    void doInstruction(IMemory& memory)
    {
        _readableInstructionStream.str({}); // reset

        // _readableInstructionStream <<  "[" << std::hex << 
        //     static_cast<int>(memory.getCurrentOpCode()) << "] ";
        doInstructionImpl(memory);
        BOOST_LOG_TRIVIAL(debug) << _readableInstructionStream.str();
    };

    virtual void doInstructionImpl(IMemory& memory) = 0;

    int _cycles;
    std::stringstream _readableInstructionStream;
};
#endif /*IINSTRUCTIONS*/
