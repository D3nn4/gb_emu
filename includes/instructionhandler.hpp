#ifndef _INSTRUCTIONHANDLER_
#define _INSTRUCTIONHANDLER_

#include <map>
#include <memory>
#include "iinstructionhandler.hpp"
#include "instructions.hpp"

class InstructionHandler : public IInstructionHandler
{
public:

    InstructionHandler(IMemory& memory);
    int doInstruction(uint8_t opCode) override;

    class InstructionException : public std::exception
    {
    public:
        InstructionException(std::string const & error)
            :_error(error){}

        const char * what () const throw ()
        {
            return _error.c_str();
        }

    private:
        std::string _error;
    };

private:

    std::map<uint8_t, std::shared_ptr<IInstructions>> _instructions =
        {
            {0x00, std::make_shared<NOP>(4)},
            {0x01, std::make_shared<Load16NextBitToRegister>(12, IMemory::REG16BIT::BC)},
            {0x02, std::make_shared<Load8BitRegValueToAdressInReg>(8, IMemory::REG16BIT::BC, IMemory::REG8BIT::A, 0)},
            {0x11, std::make_shared<Load16NextBitToRegister>(12, IMemory::REG16BIT::DE)},
            {0x12, std::make_shared<Load8BitRegValueToAdressInReg>(8, IMemory::REG16BIT::DE, IMemory::REG8BIT::A, 0)},
            {0x21, std::make_shared<Load16NextBitToRegister>(12, IMemory::REG16BIT::HL)},
            {0x22, std::make_shared<Load8BitRegValueToAdressInReg>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::A, 1)},
            {0x31, std::make_shared<Load16NextBitToRegister>(12, IMemory::REG16BIT::SP)},
            {0x32, std::make_shared<Load8BitRegValueToAdressInReg>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::A, -1)},
            {0x40, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::B)},
            {0x70, std::make_shared<Load8BitRegValueToAdressInReg>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::B, 0)},
            {0x71, std::make_shared<Load8BitRegValueToAdressInReg>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::C, 0)},
            {0x72, std::make_shared<Load8BitRegValueToAdressInReg>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::D, 0)},
            {0x73, std::make_shared<Load8BitRegValueToAdressInReg>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::E, 0)},
            {0x74, std::make_shared<Load8BitRegValueToAdressInReg>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::H, 0)},
            {0x75, std::make_shared<Load8BitRegValueToAdressInReg>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::L, 0)},
            {0x77, std::make_shared<Load8BitRegValueToAdressInReg>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::A, 0)},
        };

    std::map<uint8_t, IInstructions&> _binaryInstructions;

    IMemory& _memory;
};
#endif /*INSTRUCTIONHANDLER*/
