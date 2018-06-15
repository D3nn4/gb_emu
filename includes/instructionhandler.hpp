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

    //RR == 16bitReg   NN == next16Bit
    //R == 8bitReg     N == next8Bit
    //ARR == Adress in 16BitReg    ANN == Adress in next16Bit AN == Adress in 0xff00 + N
    std::map<uint8_t, std::shared_ptr<IInstructions>> _instructions =
        {
            {0x00, std::make_shared<NOP>(4)},
            {0x01, std::make_shared<LD_RR_NN>(12, IMemory::REG16BIT::BC)},
            {0x02, std::make_shared<LD_ARR_R>(8, IMemory::REG16BIT::BC, IMemory::REG8BIT::A, 0)},
            {0x03, std::make_shared<INC_DEC_RR>(8, IMemory::REG16BIT::BC, 1)},
            {0x04, std::make_shared<INC_DEC_R>(4, IMemory::REG8BIT::B, 1)},
            {0x05, std::make_shared<INC_DEC_R>(4, IMemory::REG8BIT::B, -1)},
            {0x06, std::make_shared<LD_R_N>(8, IMemory::REG8BIT::B)},
            {0x08, std::make_shared<LD_ANN_RR>(20, IMemory::REG16BIT::SP)},
            {0x09, std::make_shared<ADD_RR>(8, IMemory::REG16BIT::BC)},
            {0x0A, std::make_shared<LD_R_ARR>(8, IMemory::REG8BIT::A, IMemory::REG16BIT::BC, 0)},
            {0x0B, std::make_shared<INC_DEC_RR>(8, IMemory::REG16BIT::BC, -1)},
            {0x0C, std::make_shared<INC_DEC_R>(4, IMemory::REG8BIT::C, 1)},
            {0x0D, std::make_shared<INC_DEC_R>(4, IMemory::REG8BIT::C, -1)},
            {0x0E, std::make_shared<LD_R_N>(8, IMemory::REG8BIT::C)},
            {0x11, std::make_shared<LD_RR_NN>(12, IMemory::REG16BIT::DE)},
            {0x12, std::make_shared<LD_ARR_R>(8, IMemory::REG16BIT::DE, IMemory::REG8BIT::A, 0)},
            {0x13, std::make_shared<INC_DEC_RR>(8, IMemory::REG16BIT::DE, 1)},
            {0x14, std::make_shared<INC_DEC_R>(4, IMemory::REG8BIT::D, 1)},
            {0x15, std::make_shared<INC_DEC_R>(4, IMemory::REG8BIT::D, -1)},
            {0x16, std::make_shared<LD_R_N>(8, IMemory::REG8BIT::D)},
            {0x19, std::make_shared<ADD_RR>(8, IMemory::REG16BIT::DE)},
            {0x1A, std::make_shared<LD_R_ARR>(8, IMemory::REG8BIT::A, IMemory::REG16BIT::DE, 0)},
            {0x1B, std::make_shared<INC_DEC_RR>(8, IMemory::REG16BIT::DE, -1)},
            {0x1C, std::make_shared<INC_DEC_R>(4, IMemory::REG8BIT::E, 1)},
            {0x1D, std::make_shared<INC_DEC_R>(4, IMemory::REG8BIT::E, -1)},
            {0x1E, std::make_shared<LD_R_N>(8, IMemory::REG8BIT::E)},
            {0x21, std::make_shared<LD_RR_NN>(12, IMemory::REG16BIT::HL)},
            {0x22, std::make_shared<LD_ARR_R>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::A, 1)},
            {0x23, std::make_shared<INC_DEC_RR>(8, IMemory::REG16BIT::HL, 1)},
            {0x24, std::make_shared<INC_DEC_R>(4, IMemory::REG8BIT::H, 1)},
            {0x25, std::make_shared<INC_DEC_R>(4, IMemory::REG8BIT::H, -1)},
            {0x26, std::make_shared<LD_R_N>(8, IMemory::REG8BIT::H)},
            {0x29, std::make_shared<ADD_RR>(8, IMemory::REG16BIT::HL)},
            {0x2A, std::make_shared<LD_R_ARR>(8, IMemory::REG8BIT::A, IMemory::REG16BIT::HL, 1)},
            {0x2B, std::make_shared<INC_DEC_RR>(8, IMemory::REG16BIT::HL, -1)},
            {0x2C, std::make_shared<INC_DEC_R>(4, IMemory::REG8BIT::L, 1)},
            {0x2D, std::make_shared<INC_DEC_R>(4, IMemory::REG8BIT::L, -1)},
            {0x2E, std::make_shared<LD_R_N>(8, IMemory::REG8BIT::L)},
            {0x31, std::make_shared<LD_RR_NN>(12, IMemory::REG16BIT::SP)},
            {0x32, std::make_shared<LD_ARR_R>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::A, -1)},
            {0x33, std::make_shared<INC_DEC_RR>(8, IMemory::REG16BIT::SP, 1)},
            {0x34, std::make_shared<INC_DEC_ARR>(12, IMemory::REG16BIT::HL, 1)},
            {0x35, std::make_shared<INC_DEC_ARR>(12, IMemory::REG16BIT::HL, -1)},
            {0x36, std::make_shared<LD_ARR_N>(12, IMemory::REG16BIT::HL)},
            {0x3A, std::make_shared<LD_R_ARR>(8, IMemory::REG8BIT::A, IMemory::REG16BIT::HL, -1)},
            {0x39, std::make_shared<ADD_RR>(8, IMemory::REG16BIT::SP)},
            {0x3B, std::make_shared<INC_DEC_RR>(8, IMemory::REG16BIT::SP, -1)},
            {0x3C, std::make_shared<INC_DEC_R>(4, IMemory::REG8BIT::A, 1)},//TODO FLAGS
            {0x3D, std::make_shared<INC_DEC_R>(4, IMemory::REG8BIT::A, -1)},//TODO FLAGS
            {0x3E, std::make_shared<LD_R_N>(8, IMemory::REG8BIT::A)},
            {0x40, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::B)},
            {0x41, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::C)},
            {0x42, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::D)},
            {0x43, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::E)},
            {0x44, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::H)},
            {0x45, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::L)},
            {0x46, std::make_shared<LD_R_ARR>(8, IMemory::REG8BIT::B, IMemory::REG16BIT::HL, 0)},
            {0x47, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::B, IMemory::REG8BIT::A)},
            {0x48, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::C, IMemory::REG8BIT::B)},
            {0x49, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::C, IMemory::REG8BIT::C)},
            {0x4A, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::C, IMemory::REG8BIT::D)},
            {0x4B, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::C, IMemory::REG8BIT::E)},
            {0x4C, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::C, IMemory::REG8BIT::H)},
            {0x4D, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::C, IMemory::REG8BIT::L)},
            {0x4E, std::make_shared<LD_R_ARR>(8, IMemory::REG8BIT::C, IMemory::REG16BIT::HL, 0)},
            {0x4F, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::C, IMemory::REG8BIT::A)},
            {0x50, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::D, IMemory::REG8BIT::B)},
            {0x51, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::D, IMemory::REG8BIT::C)},
            {0x52, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::D, IMemory::REG8BIT::D)},
            {0x53, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::D, IMemory::REG8BIT::E)},
            {0x54, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::D, IMemory::REG8BIT::H)},
            {0x55, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::D, IMemory::REG8BIT::L)},
            {0x56, std::make_shared<LD_R_ARR>(8, IMemory::REG8BIT::D, IMemory::REG16BIT::HL, 0)},
            {0x57, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::D, IMemory::REG8BIT::A)},
            {0x58, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::E, IMemory::REG8BIT::B)},
            {0x59, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::E, IMemory::REG8BIT::C)},
            {0x5A, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::E, IMemory::REG8BIT::D)},
            {0x5B, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::E, IMemory::REG8BIT::E)},
            {0x5C, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::E, IMemory::REG8BIT::H)},
            {0x5D, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::E, IMemory::REG8BIT::L)},
            {0x5E, std::make_shared<LD_R_ARR>(8, IMemory::REG8BIT::E, IMemory::REG16BIT::HL, 0)},
            {0x5F, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::E, IMemory::REG8BIT::A)},
            {0x60, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::H, IMemory::REG8BIT::B)},
            {0x61, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::H, IMemory::REG8BIT::C)},
            {0x62, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::H, IMemory::REG8BIT::D)},
            {0x63, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::H, IMemory::REG8BIT::E)},
            {0x64, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::H, IMemory::REG8BIT::H)},
            {0x65, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::H, IMemory::REG8BIT::L)},
            {0x66, std::make_shared<LD_R_ARR>(8, IMemory::REG8BIT::H, IMemory::REG16BIT::HL, 0)},
            {0x67, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::H, IMemory::REG8BIT::A)},
            {0x68, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::L, IMemory::REG8BIT::B)},
            {0x69, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::L, IMemory::REG8BIT::C)},
            {0x6A, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::L, IMemory::REG8BIT::D)},
            {0x6B, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::L, IMemory::REG8BIT::E)},
            {0x6C, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::L, IMemory::REG8BIT::H)},
            {0x6D, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::L, IMemory::REG8BIT::L)},
            {0x6E, std::make_shared<LD_R_ARR>(8, IMemory::REG8BIT::L, IMemory::REG16BIT::HL, 0)},
            {0x6F, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::L, IMemory::REG8BIT::A)},
            {0x78, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::A, IMemory::REG8BIT::B)},
            {0x79, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::A, IMemory::REG8BIT::C)},
            {0x7A, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::A, IMemory::REG8BIT::D)},
            {0x7B, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::A, IMemory::REG8BIT::E)},
            {0x7C, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::A, IMemory::REG8BIT::H)},
            {0x7D, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::A, IMemory::REG8BIT::L)},
            {0x7E, std::make_shared<LD_R_ARR>(8, IMemory::REG8BIT::A, IMemory::REG16BIT::HL, 0)},
            {0x7F, std::make_shared<LD_R_R>(4, IMemory::REG8BIT::A, IMemory::REG8BIT::A)},
            {0x70, std::make_shared<LD_ARR_R>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::B, 0)},
            {0x71, std::make_shared<LD_ARR_R>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::C, 0)},
            {0x72, std::make_shared<LD_ARR_R>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::D, 0)},
            {0x73, std::make_shared<LD_ARR_R>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::E, 0)},
            {0x74, std::make_shared<LD_ARR_R>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::H, 0)},
            {0x75, std::make_shared<LD_ARR_R>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::L, 0)},
            {0x77, std::make_shared<LD_ARR_R>(8, IMemory::REG16BIT::HL, IMemory::REG8BIT::A, 0)},
            {0x80, std::make_shared<ADD_R>(4, IMemory::REG8BIT::B)},
            {0x81, std::make_shared<ADD_R>(4, IMemory::REG8BIT::C)},
            {0x82, std::make_shared<ADD_R>(4, IMemory::REG8BIT::D)},
            {0x83, std::make_shared<ADD_R>(4, IMemory::REG8BIT::E)},
            {0x84, std::make_shared<ADD_R>(4, IMemory::REG8BIT::H)},
            {0x85, std::make_shared<ADD_R>(4, IMemory::REG8BIT::L)},
            {0x86, std::make_shared<ADD_ARR>(8)},
            {0x87, std::make_shared<ADD_R>(4, IMemory::REG8BIT::A)},
            {0x88, std::make_shared<ADC_R>(4, IMemory::REG8BIT::B)},
            {0x89, std::make_shared<ADC_R>(4, IMemory::REG8BIT::C)},
            {0x8A, std::make_shared<ADC_R>(4, IMemory::REG8BIT::D)},
            {0x8B, std::make_shared<ADC_R>(4, IMemory::REG8BIT::E)},
            {0x8C, std::make_shared<ADC_R>(4, IMemory::REG8BIT::H)},
            {0x8D, std::make_shared<ADC_R>(4, IMemory::REG8BIT::L)},
            {0x8E, std::make_shared<ADC_ARR>(8)},
            {0x8F, std::make_shared<ADC_R>(4, IMemory::REG8BIT::A)},
            {0xA0, std::make_shared<AND_R>(4, IMemory::REG8BIT::B)},
            {0xA1, std::make_shared<AND_R>(4, IMemory::REG8BIT::C)},
            {0xA2, std::make_shared<AND_R>(4, IMemory::REG8BIT::D)},
            {0xA3, std::make_shared<AND_R>(4, IMemory::REG8BIT::E)},
            {0xA4, std::make_shared<AND_R>(4, IMemory::REG8BIT::H)},
            {0xA5, std::make_shared<AND_R>(4, IMemory::REG8BIT::L)},
            {0xA6, std::make_shared<AND_ARR>(8, IMemory::REG16BIT::HL)},
            {0xA7, std::make_shared<AND_R>(4, IMemory::REG8BIT::A)},
            {0xA8, std::make_shared<XOR_R>(4, IMemory::REG8BIT::B)},
            {0xA9, std::make_shared<XOR_R>(4, IMemory::REG8BIT::C)},
            {0xAA, std::make_shared<XOR_R>(4, IMemory::REG8BIT::D)},
            {0xAB, std::make_shared<XOR_R>(4, IMemory::REG8BIT::E)},
            {0xAC, std::make_shared<XOR_R>(4, IMemory::REG8BIT::H)},
            {0xAD, std::make_shared<XOR_R>(4, IMemory::REG8BIT::L)},
            {0xAE, std::make_shared<XOR_ARR>(8, IMemory::REG16BIT::HL)},
            {0xAF, std::make_shared<XOR_R>(4, IMemory::REG8BIT::A)},
            {0xB0, std::make_shared<OR_R>(4, IMemory::REG8BIT::B)},
            {0xB1, std::make_shared<OR_R>(4, IMemory::REG8BIT::C)},
            {0xB2, std::make_shared<OR_R>(4, IMemory::REG8BIT::D)},
            {0xB3, std::make_shared<OR_R>(4, IMemory::REG8BIT::E)},
            {0xB4, std::make_shared<OR_R>(4, IMemory::REG8BIT::H)},
            {0xB5, std::make_shared<OR_R>(4, IMemory::REG8BIT::L)},
            {0xB6, std::make_shared<OR_ARR>(8, IMemory::REG16BIT::HL)},
            {0xB7, std::make_shared<OR_R>(4, IMemory::REG8BIT::A)},
            {0xC1, std::make_shared<POP_RR>(12, IMemory::REG16BIT::BC)},
            {0xC5, std::make_shared<PUSH_RR>(16, IMemory::REG16BIT::BC)},
            {0xC6, std::make_shared<ADD_N>(8)},
            {0xCE, std::make_shared<ADC_N>(8)},
            {0xD1, std::make_shared<POP_RR>(12, IMemory::REG16BIT::DE)},
            {0xD5, std::make_shared<PUSH_RR>(16, IMemory::REG16BIT::DE)},
            {0xE0, std::make_shared<LDH_AN_R>(12, IMemory::REG8BIT::A)},
            {0xE1, std::make_shared<POP_RR>(12, IMemory::REG16BIT::HL)},
            {0xE2, std::make_shared<LDH_AR_R>(8, IMemory::REG8BIT::C, IMemory::REG8BIT::A)},
            {0xE5, std::make_shared<PUSH_RR>(16, IMemory::REG16BIT::HL)},
            {0xE6, std::make_shared<AND_N>(8)},
            {0xEA, std::make_shared<LD_ANN_R>(16, IMemory::REG8BIT::A)},
            {0xEE, std::make_shared<XOR_N>(8)},
            {0xF0, std::make_shared<LDH_R_AN>(12, IMemory::REG8BIT::A)},
            {0xF1, std::make_shared<POP_RR>(12, IMemory::REG16BIT::AF)},
            {0xF2, std::make_shared<LDH_R_AR>(8, IMemory::REG8BIT::A, IMemory::REG8BIT::C)},
            {0xF5, std::make_shared<PUSH_RR>(16, IMemory::REG16BIT::AF)},
            {0xF6, std::make_shared<OR_N>(8)},
            {0xF9, std::make_shared<LD_RR_RR>(8, IMemory::REG16BIT::SP, IMemory::REG16BIT::HL)},
            {0xFA, std::make_shared<LD_R_ANN>(16, IMemory::REG8BIT::A)},
        };

    std::map<uint8_t, IInstructions&> _binaryInstructions;

    IMemory& _memory;
};
#endif /*INSTRUCTIONHANDLER*/
