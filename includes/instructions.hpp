#ifndef _INSTRUCTIONS_
#define _INSTRUCTIONS_

#include <iostream>
#include "iinstructions.hpp"

//RR  == 16bitReg   NN == next16Bit
//R   == 8bitReg     N == next8Bit
//ARR == Adress in 16BitReg    ANN == Adress in next16Bit
//AR  == Adress in 0xff00 + R       AN == Adress in 0xff00 + N

//OpCode 0x00
class NOP : public IInstructions
{
public :
    NOP(int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }
};

//OpCode 0x01 0x11 0x21 0x31
class LD_RR_NN : public IInstructions
{
public:
    LD_RR_NN(int cycles, IMemory::REG16BIT reg)
        :IInstructions(cycles),
         _register(reg){};

    void doInstruction(IMemory& memory) override {
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        IMemory::RomData const & rom = memory.getReadOnlyMemory();
        uint16_t value = ((uint16_t)rom[cursor + 2] << 8) | rom[cursor + 1];
        memory.set16BitRegister(_register, value);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 3);
    }

    IMemory::REG16BIT _register;
};

//OpCode 0x06 0x0E 0x16 0x1E 0x26 0x2E 0x3E
class LD_R_N : public IInstructions
{
public:
    LD_R_N(int cycles, IMemory::REG8BIT reg)
        :IInstructions(cycles),
         _register(reg){};

    void doInstruction(IMemory& memory) override {
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        IMemory::RomData const & rom = memory.getReadOnlyMemory();
        uint16_t value = rom[cursor + 1];
        memory.set8BitRegister(_register, value);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 2);
    }

    IMemory::REG8BIT _register;
};

//OpCode increment 0x04 0x05 0x0C 0x14 0x15 0x1C 0x1D 0x24 0x25 0x2C 0x2D 0x3C 0x3D
class INC_DEC_R : public IInstructions
{
public:
    INC_DEC_R(int cycles, IMemory::REG8BIT reg8Bit, int value)
        :IInstructions(cycles),
         _reg8Bit(reg8Bit),
         _value(value){};

    void doInstruction(IMemory& memory) override {
        uint8_t regValue = memory.get8BitRegister(_reg8Bit);
        memory.set8BitRegister(_reg8Bit, regValue + _value);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }

    IMemory::REG8BIT _reg8Bit;
    int _value;
};

//OpCode inc/dec 0x03 0x0B 0x13 0x1B 0x23 0x2B 0x33 0x3B
class INC_DEC_RR : public IInstructions
{
public:
    INC_DEC_RR(int cycles, IMemory::REG16BIT reg16Bit, int value)
        :IInstructions(cycles),
         _reg16Bit(reg16Bit),
         _value(value){};

    void doInstruction(IMemory& memory) override {
        uint16_t regValue = memory.get16BitRegister(_reg16Bit);
        memory.set16BitRegister(_reg16Bit, regValue + _value);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }

    IMemory::REG16BIT _reg16Bit;
    int _value;
};

//OpCode inc/dec 0x34 0x35
class INC_DEC_ARR : public IInstructions
{
public:
   INC_DEC_ARR(int cycles, IMemory::REG16BIT reg16Bit, int value)
        :IInstructions(cycles),
         _reg16Bit(reg16Bit),
         _value(value){};

    void doInstruction(IMemory& memory) override {
        uint16_t adress = memory.get16BitRegister(_reg16Bit);
        IMemory::RomData const rom = memory.getReadOnlyMemory();
        uint8_t valueToIncrement = rom[adress] + _value;
        memory.writeInROM(valueToIncrement, adress);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }

    IMemory::REG16BIT _reg16Bit;
    int _value;
};

//opCode 0x0A 0x1A 0x2A 0x3A 0x46 0x4E 0x56 ox5E 0x66 0x6E 0x7E
class LD_R_ARR : public IInstructions
{
public:
    LD_R_ARR(int cycles, IMemory::REG8BIT reg8Bit, IMemory::REG16BIT reg16Bit, int addTo16BitReg)
        :IInstructions(cycles),
         _16BitReg(reg16Bit),
         _8BitReg(reg8Bit),
         _addTo16BitReg(addTo16BitReg){};

    void doInstruction(IMemory& memory) override {
        uint16_t adress = memory.get16BitRegister(_16BitReg);
        IMemory::RomData rom = memory.getReadOnlyMemory();
        memory.set8BitRegister(_8BitReg, rom[adress]);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
        if (_addTo16BitReg != 0) {
            adress += _addTo16BitReg;
            memory.set16BitRegister(_16BitReg, adress);
        }
    }

    IMemory::REG16BIT _16BitReg;
    IMemory::REG8BIT _8BitReg;
    int _addTo16BitReg;
};

//OpCode 0x02 0x12 0x22 0x32 0x70 0x71 0x72 0x73 0x74 0x75 0x77
class LD_ARR_R : public IInstructions
{
public:
   LD_ARR_R(int cycles, IMemory::REG16BIT reg16Bit, IMemory::REG8BIT reg8Bit, int addTo16BitReg)
        :IInstructions(cycles),
         _16BitReg(reg16Bit),
         _8BitReg(reg8Bit),
         _addTo16BitReg(addTo16BitReg){};

    void doInstruction(IMemory& memory) override {
        uint8_t reg8BitValue = memory.get8BitRegister(_8BitReg);
        uint16_t reg16BitValue = memory.get16BitRegister(_16BitReg);
        memory.writeInROM(reg8BitValue, reg16BitValue);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
        if (_addTo16BitReg != 0) {
            reg16BitValue += _addTo16BitReg;
            memory.set16BitRegister(_16BitReg, reg16BitValue);
        }
    }

    IMemory::REG16BIT _16BitReg;
    IMemory::REG8BIT _8BitReg;
    int _addTo16BitReg;
};

//OpCode 0x40 to 0x45 0x47 to 0x4D 0x4F 0x50 to 0x55 0x57 to 0x5D 0x5F 0x60 to 0x65 0x67 to 0x6D 0x6F 0x78 to 0x7D 0x7F
class LD_R_R : public IInstructions
{
public:
    LD_R_R(int cycles, IMemory::REG8BIT toCopyTo, IMemory::REG8BIT toCopyFrom)
        :IInstructions(cycles),
         _toCopyTo(toCopyTo),
         _toCopyFrom(toCopyFrom){};

    void doInstruction(IMemory& memory) override {
        uint8_t valueToCopy = memory.get8BitRegister(_toCopyFrom);
        memory.set8BitRegister(_toCopyTo, valueToCopy);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }

    IMemory::REG8BIT _toCopyTo;
    IMemory::REG8BIT _toCopyFrom;
};

//OpCode 0x36
class LD_ARR_N : public IInstructions
{
public:
    LD_ARR_N (int cycles, IMemory::REG16BIT reg16Bit)
        :IInstructions(cycles),
         _16BitReg(reg16Bit){};

    void doInstruction(IMemory& memory) override {
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        uint16_t adress = memory.get16BitRegister(_16BitReg);
        IMemory::RomData rom = memory.getReadOnlyMemory();
        uint8_t valueToLoad = rom[cursor + 1];
        memory.writeInROM(valueToLoad, adress);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 2);
    }

    IMemory::REG16BIT _16BitReg;
};

//OpCode 0x08
class LD_NN_RR : public IInstructions
{
public:
    LD_NN_RR (int cycles, IMemory::REG16BIT reg16Bit)
        :IInstructions(cycles),
         _16BitReg(reg16Bit){};

    void doInstruction(IMemory& memory) override {
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        IMemory::RomData rom = memory.getReadOnlyMemory();
        uint16_t adress = ((uint16_t)rom[cursor + 2] << 8) | rom[cursor + 1];
        uint16_t reg16BitValue = memory.get16BitRegister(_16BitReg);

        uint8_t mostSignificantBit = (uint8_t)(reg16BitValue >> 8) & 0xff;
        uint8_t lessSignificantBit = (uint8_t)(reg16BitValue & 0xff);
        memory.writeInROM(lessSignificantBit, adress);
        memory.writeInROM(mostSignificantBit, adress + 1);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 3);
    }

    IMemory::REG16BIT _16BitReg;
};

//OpCode 0xE0
class LDH_AN_R : public IInstructions
{
public:
    LDH_AN_R (int cycles, IMemory::REG8BIT reg8Bit)
        :IInstructions(cycles),
         _8BitReg(reg8Bit){};

    void doInstruction(IMemory& memory) override {
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        uint8_t valueToLoad = memory.get8BitRegister(_8BitReg);
        IMemory::RomData rom = memory.getReadOnlyMemory();
        uint16_t adress = 0xff00 + rom[cursor + 1];
        memory.writeInROM(valueToLoad, adress);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 2);
    }

    IMemory::REG8BIT _8BitReg;
};

//OpCode 0xF0
class LDH_R_AN : public IInstructions
{
public:
    LDH_R_AN (int cycles, IMemory::REG8BIT reg8Bit)
        :IInstructions(cycles),
         _8BitReg(reg8Bit){};

    void doInstruction(IMemory& memory) override {
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        IMemory::RomData rom = memory.getReadOnlyMemory();
        uint16_t adress = 0xff00 + rom[cursor + 1];
        uint8_t valueToLoad = rom[adress];
        memory.set8BitRegister(_8BitReg, valueToLoad);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 2);
    }

    IMemory::REG8BIT _8BitReg;
};

//OpCode 0xE2
class LDH_AR_R : public IInstructions
{
public:
    LDH_AR_R (int cycles, IMemory::REG8BIT reg8Bit, IMemory::REG8BIT reg8BitToLoad)
        :IInstructions(cycles),
         _8BitRegToLoad(reg8BitToLoad),
         _8BitReg(reg8Bit){};

    void doInstruction(IMemory& memory) override {
        uint8_t valueForAdress = memory.get8BitRegister(_8BitReg);
        uint8_t valueToLoad = memory.get8BitRegister(_8BitRegToLoad);
        uint16_t adress = 0xff00 + valueForAdress;
        memory.writeInROM(valueToLoad, adress);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }

    IMemory::REG8BIT _8BitRegToLoad;
    IMemory::REG8BIT _8BitReg;
};

//OpCode 0xF2
class LDH_R_AR : public IInstructions
{
public:
    LDH_R_AR (int cycles, IMemory::REG8BIT reg8BitToLoad, IMemory::REG8BIT reg8Bit)
        :IInstructions(cycles),
         _8BitRegToLoad(reg8BitToLoad),
         _8BitReg(reg8Bit){};

    void doInstruction(IMemory& memory) override {
        uint8_t valueForAdress = memory.get8BitRegister(_8BitReg);
        IMemory::RomData rom = memory.getReadOnlyMemory();
        uint16_t adress = 0xff00 + rom[valueForAdress];
        uint8_t valueToLoad = rom[adress];
        memory.set8BitRegister(_8BitRegToLoad, valueToLoad);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }

    IMemory::REG8BIT _8BitRegToLoad;
    IMemory::REG8BIT _8BitReg;
};


//OpCode 0xF2
class LD_RR_RR : public IInstructions
{
public:
    LD_RR_RR (int cycles, IMemory::REG16BIT reg16Bit, IMemory::REG16BIT reg16BitToCopy)
        :IInstructions(cycles),
         _16BitReg(reg16Bit),
         _16BitRegToCopy(reg16BitToCopy){};

    void doInstruction(IMemory& memory) override {
        uint16_t valueToLoad = memory.get16BitRegister(IMemory::REG16BIT::HL);
        memory.set16BitRegister(_16BitReg, valueToLoad);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }

    IMemory::REG16BIT _16BitReg;
    IMemory::REG16BIT _16BitRegToCopy;
};

//OpCode 0xEA
class LD_ANN_R : public IInstructions
{
public:
    LD_ANN_R (int cycles, IMemory::REG8BIT reg8Bit)
        :IInstructions(cycles),
         _8BitReg(reg8Bit){};

    void doInstruction(IMemory& memory) override {
        uint8_t reg8BitValue = memory.get8BitRegister(_8BitReg);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        IMemory::RomData rom = memory.getReadOnlyMemory();
        uint16_t adress = ((uint16_t)rom[cursor + 2] << 8) | rom[cursor + 1];
        memory.writeInROM(reg8BitValue, adress);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 3);
    }
    IMemory::REG8BIT _8BitReg;
};

//OpCode 0xFA
class LD_R_ANN : public IInstructions
{
public:
    LD_R_ANN (int cycles, IMemory::REG8BIT reg8Bit)
        :IInstructions(cycles),
         _8BitReg(reg8Bit){};

    void doInstruction(IMemory& memory) override {
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        IMemory::RomData rom = memory.getReadOnlyMemory();
        uint16_t adress = ((uint16_t)rom[cursor + 2] << 8) | rom[cursor + 1];
        uint8_t value = rom[adress];
        memory.set8BitRegister(_8BitReg, value);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 3);
    }
    IMemory::REG8BIT _8BitReg;
};

//OpCode 0xA8 0xA9 0xAA 0xAB 0xAC 0xAD 0xAF
class XOR_R : public IInstructions
{
public:
    XOR_R (int cycles, IMemory::REG8BIT reg8Bit)
        :IInstructions(cycles),
         _8BitReg(reg8Bit){};

    void doInstruction(IMemory& memory) override {
        uint8_t AValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint8_t reg8BitValue = memory.get8BitRegister(_8BitReg);
        uint8_t result = AValue ^ reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);

        // memory.unsetBitInRegister()
    }
    IMemory::REG8BIT _8BitReg;
};

//OpCode 0xB0 0xB1 0xB2 0xB3 0xB4 0xB5 0xB7
class OR_R : public IInstructions
{
public:
    OR_R (int cycles, IMemory::REG8BIT reg8Bit)
        :IInstructions(cycles),
         _8BitReg(reg8Bit){};

    void doInstruction(IMemory& memory) override {
        uint8_t AValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint8_t reg8BitValue = memory.get8BitRegister(_8BitReg);
        uint8_t result = AValue | reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }
    IMemory::REG8BIT _8BitReg;
};

//OpCode 0xA0 0xA1 0xA2 0xA3 0xA4 0xA5 0xA7
class AND_R : public IInstructions
{
public:
    AND_R (int cycles, IMemory::REG8BIT reg8Bit)
        :IInstructions(cycles),
         _8BitReg(reg8Bit){};

    void doInstruction(IMemory& memory) override {
        uint8_t AValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint8_t reg8BitValue = memory.get8BitRegister(_8BitReg);
        uint8_t result = AValue & reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }
    IMemory::REG8BIT _8BitReg;
};

//opCode 0xAE
class XOR_ARR : public IInstructions
{
public:
    XOR_ARR (int cycles, IMemory::REG16BIT reg16Bit)
        :IInstructions(cycles),
         _16BitReg(reg16Bit){};

    void doInstruction(IMemory& memory) override {
        uint8_t AValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint16_t adress = memory.get16BitRegister(_16BitReg);
        IMemory::RomData rom = memory.getReadOnlyMemory();
        uint8_t reg8BitValue = rom[adress];
        uint8_t result = AValue ^ reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }
    IMemory::REG16BIT _16BitReg;
};

//opCode 0xB6
class OR_ARR : public IInstructions
{
public:
    OR_ARR (int cycles, IMemory::REG16BIT reg16Bit)
        :IInstructions(cycles),
         _16BitReg(reg16Bit){};

    void doInstruction(IMemory& memory) override {
        uint8_t AValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint16_t adress = memory.get16BitRegister(_16BitReg);
        IMemory::RomData rom = memory.getReadOnlyMemory();
        uint8_t reg8BitValue = rom[adress];
        uint8_t result = AValue | reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }
    IMemory::REG16BIT _16BitReg;
};

//opCode 0xA6
class AND_ARR : public IInstructions
{
public:
    AND_ARR (int cycles, IMemory::REG16BIT reg16Bit)
        :IInstructions(cycles),
         _16BitReg(reg16Bit){};

    void doInstruction(IMemory& memory) override {
        uint8_t AValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint16_t adress = memory.get16BitRegister(_16BitReg);
        IMemory::RomData rom = memory.getReadOnlyMemory();
        uint8_t reg8BitValue = rom[adress];
        uint8_t result = AValue & reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }
    IMemory::REG16BIT _16BitReg;
};

//opCode 0xEE
class XOR_N : public IInstructions
{
public:
    XOR_N (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint8_t AValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        IMemory::RomData rom = memory.getReadOnlyMemory();
        uint8_t reg8BitValue = rom[cursor + 1];
        uint8_t result = AValue ^ reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 2);
    }
    IMemory::REG8BIT _8BitReg;
};

//opCode 0xF6
class OR_N : public IInstructions
{
public:
    OR_N (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint8_t AValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        IMemory::RomData rom = memory.getReadOnlyMemory();
        uint8_t reg8BitValue = rom[cursor + 1];
        uint8_t result = AValue | reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 2);
    }
    IMemory::REG8BIT _8BitReg;
};

//opCode 0xE6
class AND_N : public IInstructions
{
public:
    AND_N (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint8_t AValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        IMemory::RomData rom = memory.getReadOnlyMemory();
        uint8_t reg8BitValue = rom[cursor + 1];
        uint8_t result = AValue & reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 2);
    }
    IMemory::REG8BIT _8BitReg;
};
#endif /*INSTRUCTIONS*/
