#ifndef _INSTRUCTIONS_
#define _INSTRUCTIONS_

#include <functional>
#include <map>
#include "imemory.hpp"
#include "iinstructions.hpp"

typedef std::function<void(IMemory&)> funcInstruct;

using namespace std::placeholders;

class Instructions : public IInstructions
{
public:
    //TODO add cycles to somes instructions
    void load16BitToSP(IMemory& memory) override;

    std::map<uint8_t, funcInstruct> _instructions =
        {
            {0x0031, std::bind(&Instructions::load16BitToSP, this, _1)}
        };

private:

};
#endif /*IINSTRUCTIONS*/
