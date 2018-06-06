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
            {0x03, std::make_shared<addValueTo16BitRegister>(8, IMemory::REG16BIT::BC, 1)},
            {0x04, std::make_shared<addValueTo8BitRegister>(4, IMemory::REG8BIT::B, 1)},//TODO FLAGS
            {0x05, std::make_shared<addValueTo8BitRegister>(4, IMemory::REG8BIT::B, -1)},//TODO FLAGS
            {0x0B, std::make_shared<addValueTo16BitRegister>(8, IMemory::REG16BIT::BC, -1)},
            {0x0C, std::make_shared<addValueTo8BitRegister>(4, IMemory::REG8BIT::C, 1)},//TODO FLAGS
            {0x0D, std::make_shared<addValueTo8BitRegister>(4, IMemory::REG8BIT::C, -1)},//TODO FLAGS
            {0x11, std::make_shared<Load16NextBitToRegister>(12, IMemory::REG16BIT::DE)},
            {0x12, std::make_shared<Load8BitRegValueToAdressInReg>(8, IMemory::REG16BIT::DE, IMemory::REG8BIT::A, 0)},
            {0x13, std::make_shared<addValueTo16BitRegister>(8, IMemory::REG16BIT::DE, 1)},
            {0x14, std::make_shared<addValueTo8BitRegister>(4, IMemory::REG8BIT::D, 1)},//TODO FLAGS
            {0x15, std::make_shared<addValueTo8BitRegister>(4, IMemory::REG8BIT::D, -1)},//TODO FLAGS
            {0x1B, std::make_shared<addValueTo16BitRegister>(8, IMemory::REG16BIT::DE, -1)},
            {0x1C, std::make_shared<addValueTo8BitRegister>(4, IMemory::REG8BIT::E, 1)},//TODO FLAGS
            {0x1D, std::make_shared<addValueTo8BitRegister>(4, IMemory::REG8BIT::E, -1)},//TODO FLAGS
            {0x21, std::make_shared<Load16NextBitToRegister>(12, IMemory::REG16BIT::HL)},
            {0x22, std::make_shared<Load8BitRegValueToAdressInReg>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::A, 1)},
            {0x23, std::make_shared<addValueTo16BitRegister>(8, IMemory::REG16BIT::HL, 1)},
            {0x24, std::make_shared<addValueTo8BitRegister>(4, IMemory::REG8BIT::H, 1)},//TODO FLAGS
            {0x25, std::make_shared<addValueTo8BitRegister>(4, IMemory::REG8BIT::H, -1)},//TODO FLAGS
            {0x2B, std::make_shared<addValueTo16BitRegister>(8, IMemory::REG16BIT::HL, -1)},
            {0x2C, std::make_shared<addValueTo8BitRegister>(4, IMemory::REG8BIT::L, 1)},//TODO FLAGS
            {0x2D, std::make_shared<addValueTo8BitRegister>(4, IMemory::REG8BIT::L, -1)},//TODO FLAGS
            {0x31, std::make_shared<Load16NextBitToRegister>(12, IMemory::REG16BIT::SP)},
            {0x32, std::make_shared<Load8BitRegValueToAdressInReg>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::A, -1)},
            {0x33, std::make_shared<addValueTo16BitRegister>(8, IMemory::REG16BIT::SP, 1)},
            {0x34, std::make_shared<addValueAtAdressInReg>(12, IMemory::REG16BIT::HL, 1)},
            {0x35, std::make_shared<addValueAtAdressInReg>(12, IMemory::REG16BIT::HL, -1)},
            {0x3B, std::make_shared<addValueTo16BitRegister>(8, IMemory::REG16BIT::SP, -1)},
            {0x3C, std::make_shared<addValueTo8BitRegister>(4, IMemory::REG8BIT::A, 1)},//TODO FLAGS
            {0x3D, std::make_shared<addValueTo8BitRegister>(4, IMemory::REG8BIT::A, -1)},//TODO FLAGS
            {0x40, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::B)},
            {0x41, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::C)},
            {0x42, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::D)},
            {0x43, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::E)},
            {0x44, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::H)},
            {0x45, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::L)},
            {0x47, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::A)},
            {0x48, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::C, IMemory::REG8BIT::B)},
            {0x49, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::C, IMemory::REG8BIT::C)},
            {0x4A, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::C, IMemory::REG8BIT::D)},
            {0x4B, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::C, IMemory::REG8BIT::E)},
            {0x4C, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::C, IMemory::REG8BIT::H)},
            {0x4D, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::C, IMemory::REG8BIT::L)},
            {0x4F, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::C, IMemory::REG8BIT::A)},
            {0x50, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::D, IMemory::REG8BIT::B)},
            {0x51, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::D, IMemory::REG8BIT::C)},
            {0x52, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::D, IMemory::REG8BIT::D)},
            {0x53, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::D, IMemory::REG8BIT::E)},
            {0x54, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::D, IMemory::REG8BIT::H)},
            {0x55, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::D, IMemory::REG8BIT::L)},
            {0x57, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::D, IMemory::REG8BIT::A)},
            {0x58, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::E, IMemory::REG8BIT::B)},
            {0x59, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::E, IMemory::REG8BIT::C)},
            {0x5A, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::E, IMemory::REG8BIT::D)},
            {0x5B, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::E, IMemory::REG8BIT::E)},
            {0x5C, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::E, IMemory::REG8BIT::H)},
            {0x5D, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::E, IMemory::REG8BIT::L)},
            {0x5F, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::E, IMemory::REG8BIT::A)},
            {0x60, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::H, IMemory::REG8BIT::B)},
            {0x61, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::H, IMemory::REG8BIT::C)},
            {0x62, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::H, IMemory::REG8BIT::D)},
            {0x63, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::H, IMemory::REG8BIT::E)},
            {0x64, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::H, IMemory::REG8BIT::H)},
            {0x65, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::H, IMemory::REG8BIT::L)},
            {0x67, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::H, IMemory::REG8BIT::A)},
            {0x68, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::L, IMemory::REG8BIT::B)},
            {0x69, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::L, IMemory::REG8BIT::C)},
            {0x6A, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::L, IMemory::REG8BIT::D)},
            {0x6B, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::L, IMemory::REG8BIT::E)},
            {0x6C, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::L, IMemory::REG8BIT::H)},
            {0x6D, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::L, IMemory::REG8BIT::L)},
            {0x6F, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::L, IMemory::REG8BIT::A)},
            {0x78, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::A, IMemory::REG8BIT::B)},
            {0x79, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::A, IMemory::REG8BIT::C)},
            {0x7A, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::A, IMemory::REG8BIT::D)},
            {0x7B, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::A, IMemory::REG8BIT::E)},
            {0x7C, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::A, IMemory::REG8BIT::H)},
            {0x7D, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::A, IMemory::REG8BIT::L)},
            {0x7F, std::make_shared<Load8BitRegValueTo8BitRegister>(4, IMemory::REG8BIT::A, IMemory::REG8BIT::A)},
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
