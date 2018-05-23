#ifndef _INSTRUCTIONS_
#define _INSTRUCTIONS_

#include <functional>
#include <map>
#include "imemory.hpp"

typedef std::function<int(IMemory&)> funcInstruct;

using namespace std::placeholders;

class Instructions 
{
public:

    int load16BitToHL(IMemory& memory);
    int load16BitToSP(IMemory& memory);
    int xorRegisterA(IMemory& memory);
    int loadAToAdressInHLAndDecrement(IMemory& memory);
    int doBinaryInstructions(IMemory& memory);

    std::map<uint8_t, funcInstruct> _instructions =
        {
            {0x21, std::bind(&Instructions::load16BitToHL, this, _1)},
            {0x31, std::bind(&Instructions::load16BitToSP, this, _1)},
            {0x32, std::bind(&Instructions::loadAToAdressInHLAndDecrement, this, _1)},
            {0xAF, std::bind(&Instructions::xorRegisterA, this, _1)},
            {0xCB, std::bind(&Instructions::doBinaryInstructions, this, _1)},
        };

private:

    uint16_t combine8BitTo16Bit(uint8_t lhs, uint8_t rhs);
    void load16NextBitToRegister(IMemory::REG16BIT reg, IMemory& memory);
    // void load8NextBitToRegister(IMemory::REG8BIT reg);
    void xor8BitRegister(IMemory::REG8BIT reg, IMemory& memory);
    void load8BitInRegisterAtAdress(IMemory::REG8BIT reg, IMemory::REG16BIT adress, IMemory& memory);
};
#endif /*IINSTRUCTIONS*/
