#ifndef _INSTRUCTIONS_
#define _INSTRUCTIONS_

#include <iostream>
#include "iinstructions.hpp"

//RR  == 16bitReg   NN == next16Bit
//R   == 8bitReg     N == next8Bit
//ARR == Adress in 16BitReg    ANN == Adress in next16Bit
//AR  == Adress in 0xff00 + R       AN == Adress in 0xff00 + N

//OpCode 0x06 0x0E 0x16 0x1E 0x26 0x2E 0x3E
class LD_R_N : public IInstructions
{
public:
    LD_R_N(int cycles, IMemory::REG8BIT reg)
        :IInstructions(cycles),
         _register(reg){};

    void doInstruction(IMemory& memory) override {
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        uint16_t valueToLoad = memory.readInMemory(cursor + 1);
        memory.set8BitRegister(_register, valueToLoad);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 2);
    }

    IMemory::REG8BIT _register;
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
        uint8_t valueToLoad = memory.readInMemory(adress);
        memory.set8BitRegister(_8BitReg, valueToLoad);
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
        uint8_t valueToLoad = memory.readInMemory(cursor + 1);
        memory.writeInROM(valueToLoad, adress);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 2);
    }

    IMemory::REG16BIT _16BitReg;
};

//OpCode 0x08
class LD_ANN_RR : public IInstructions
{
public:
    LD_ANN_RR (int cycles, IMemory::REG16BIT reg16Bit)
        :IInstructions(cycles),
         _16BitReg(reg16Bit){};

    void doInstruction(IMemory& memory) override {
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        uint8_t MS = memory.readInMemory(cursor + 2);
        uint8_t LS = memory.readInMemory(cursor + 1);
        uint16_t adress = ((uint16_t) MS << 8) | LS;
        uint16_t reg16BitValue = memory.get16BitRegister(_16BitReg);

        uint8_t mostSignificantBit = (uint8_t)(reg16BitValue >> 8) & 0xff;
        uint8_t lessSignificantBit = (uint8_t)(reg16BitValue & 0xff);
        memory.writeInROM(lessSignificantBit, adress);
        memory.writeInROM(mostSignificantBit, adress + 1);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 3);
    }

    IMemory::REG16BIT _16BitReg;
};

//OpCode 0xF9
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
        uint8_t MS = memory.readInMemory(cursor + 2);
        uint8_t LS = memory.readInMemory(cursor + 1);
        uint16_t adress = ((uint16_t) MS << 8) | LS;
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
        uint8_t MS = memory.readInMemory(cursor + 2);
        uint8_t LS = memory.readInMemory(cursor + 1);
        uint16_t adress = ((uint16_t) MS << 8) | LS;
        uint8_t value = memory.readInMemory(adress);
        memory.set8BitRegister(_8BitReg, value);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 3);
    }
    IMemory::REG8BIT _8BitReg;
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
        uint16_t adress = 0xff00 + memory.readInMemory(cursor + 1);
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
        uint16_t adress = 0xff00 + memory.readInMemory(cursor + 1);
        uint8_t valueToLoad = memory.readInMemory(adress);
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
        uint16_t adress = 0xff00 + valueForAdress;
        uint8_t valueToLoad = memory.readInMemory(adress);
        memory.set8BitRegister(_8BitRegToLoad, valueToLoad);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }

    IMemory::REG8BIT _8BitRegToLoad;
    IMemory::REG8BIT _8BitReg;
};

////////////////////////////////////
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
        uint8_t MS = memory.readInMemory(cursor + 2);
        uint8_t LS = memory.readInMemory(cursor + 1);
        uint16_t valueToLoad = ((uint16_t) MS << 8) | LS;
        memory.set16BitRegister(_register, valueToLoad);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 3);
    }

    IMemory::REG16BIT _register;
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
        uint8_t valueToIncrement = memory.readInMemory(adress) + _value;
        memory.writeInROM(valueToIncrement, adress);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }

    IMemory::REG16BIT _reg16Bit;
    int _value;
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

        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::N);
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::C);
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

        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::N);
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::C);
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

        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::N);
        memory.setFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::C);
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
        uint8_t reg8BitValue = memory.readInMemory(adress);
        uint8_t result = AValue ^ reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);

        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::N);
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::C);
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
        uint8_t reg8BitValue = memory.readInMemory(adress);
        uint8_t result = AValue | reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);

        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::N);
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::C);
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
        uint8_t reg8BitValue = memory.readInMemory(adress);
        uint8_t result = AValue & reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);

        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::N);
        memory.setFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::C);
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
        uint8_t reg8BitValue = memory.readInMemory(cursor + 1);
        uint8_t result = AValue ^ reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 2);

        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::N);
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::C);
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
        uint8_t reg8BitValue = memory.readInMemory(cursor + 1);
        uint8_t result = AValue | reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 2);

        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::N);
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::C);
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
        uint8_t reg8BitValue = memory.readInMemory(cursor + 1);
        uint8_t result = AValue & reg8BitValue;
        memory.set8BitRegister(IMemory::REG8BIT::A,result);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 2);

        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::N);
        memory.setFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::C);
    }
    IMemory::REG8BIT _8BitReg;
};

//opCode 0xC1 0xD1 0xE1 0xF1
class POP_RR : public IInstructions
{
public:
    POP_RR (int cycles, IMemory::REG16BIT reg16Bit)
        :IInstructions(cycles),
         _16BitReg(reg16Bit){};

    void doInstruction(IMemory& memory) override {
        uint16_t stackPointer = memory.get16BitRegister(IMemory::REG16BIT::SP);
        uint8_t mostSignificantBit = memory.readInMemory(stackPointer + 1);
        uint8_t lessSignificantBit = memory.readInMemory(stackPointer);
        uint16_t valueToLoad = ((uint16_t) mostSignificantBit << 8) | lessSignificantBit;
        memory.set16BitRegister(_16BitReg, valueToLoad);
        memory.set16BitRegister(IMemory::REG16BIT::SP, stackPointer + 2);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }
    IMemory::REG16BIT _16BitReg;
};

//opCode 0xC5 0xD5 0xE5 0xF5
class PUSH_RR : public IInstructions
{
public:
    PUSH_RR (int cycles, IMemory::REG16BIT reg16Bit)
        :IInstructions(cycles),
         _16BitReg(reg16Bit){};

    void doInstruction(IMemory& memory) override {
        uint16_t stackPointer = memory.get16BitRegister(IMemory::REG16BIT::SP);
        uint16_t valueToLoad = memory.get16BitRegister(_16BitReg);

        uint8_t mostSignificantBit = (uint8_t)(valueToLoad >> 8) & 0xff;
        uint8_t lessSignificantBit = (uint8_t)(valueToLoad & 0xff);
        memory.writeInROM(mostSignificantBit, stackPointer - 1);
        memory.writeInROM(lessSignificantBit, stackPointer - 2);

        memory.set16BitRegister(IMemory::REG16BIT::SP, stackPointer - 2);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }
    IMemory::REG16BIT _16BitReg;
};

// 0x80 0x81 0x82 0x83 0x84 0x85 0x87
class ADD_R : public IInstructions
{
public:
    ADD_R (int cycles, IMemory::REG8BIT reg8Bit)
        :IInstructions(cycles),
         _8BitReg(reg8Bit){};

    void doInstruction(IMemory& memory) override {
        uint8_t regAValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint8_t valueToAdd = memory.get8BitRegister(_8BitReg);

        uint8_t result = regAValue + valueToAdd;
        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        if ((((regAValue & 0x0F) + (valueToAdd & 0x0F)) & 0x10) == 0x10) {
            memory.setFlag(IMemory::FLAG::H);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::H);
        }
        if (((static_cast<uint16_t>(regAValue) + static_cast<uint16_t>(valueToAdd)) > 0x00ff)) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        memory.unsetFlag(IMemory::FLAG::N);

        memory.set8BitRegister(IMemory::REG8BIT::A, result);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }
    IMemory::REG8BIT _8BitReg;
};

// 0x86
class ADD_ARR : public IInstructions
{
public:
    ADD_ARR (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint8_t regAValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint16_t adress = memory.get16BitRegister(IMemory::REG16BIT::HL);
        uint8_t valueToAdd = memory.readInMemory(adress);

        uint8_t result = regAValue + valueToAdd;
        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        if ((((regAValue & 0x0F) + (valueToAdd & 0x0F)) & 0x10) == 0x10) {
            memory.setFlag(IMemory::FLAG::H);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::H);
        }
        if (((static_cast<uint16_t>(regAValue) + static_cast<uint16_t>(valueToAdd)) > 0x00ff)) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        memory.unsetFlag(IMemory::FLAG::N);

        memory.set8BitRegister(IMemory::REG8BIT::A, result);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }
    IMemory::REG8BIT _8BitReg;
};

// 0x09 0x19 0x29 0x39
class ADD_RR : public IInstructions
{
public:
    ADD_RR (int cycles, IMemory::REG16BIT reg16Bit)
        :IInstructions(cycles),
         _16BitReg(reg16Bit){};

    void doInstruction(IMemory& memory) override {
        uint16_t regAValue = memory.get16BitRegister(IMemory::REG16BIT::HL);
        uint16_t valueToAdd = memory.get16BitRegister(_16BitReg);

        uint16_t result = regAValue + valueToAdd;
        if ((((regAValue & 0x0F00) + (valueToAdd & 0x0F00)) & 0x1000) == 0x1000) {
            memory.setFlag(IMemory::FLAG::H);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::H);
        }
        if (((static_cast<uint32_t>(regAValue) + static_cast<uint32_t>(valueToAdd)) > 0xffff)) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        memory.unsetFlag(IMemory::FLAG::N);

        memory.set16BitRegister(IMemory::REG16BIT::HL, result);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }
    IMemory::REG16BIT _16BitReg;
};


// 0xC6
class ADD_N : public IInstructions
{
public:
    ADD_N (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint8_t regAValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        uint8_t valueToAdd = memory.readInMemory(cursor + 1);

        uint8_t result = regAValue + valueToAdd;
        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        if ((((regAValue & 0x0F) + (valueToAdd & 0x0F)) & 0x10) == 0x10) {
            memory.setFlag(IMemory::FLAG::H);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::H);
        }
        if (((static_cast<uint16_t>(regAValue) + static_cast<uint16_t>(valueToAdd)) > 0x00ff)) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        memory.unsetFlag(IMemory::FLAG::N);

        memory.set8BitRegister(IMemory::REG8BIT::A, result);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 2);
    }
};

// 0x88 0x89 0x8A 0x8B 0x8C 0x8D 0x8F
class ADC_R : public IInstructions
{
public:
    ADC_R (int cycles, IMemory::REG8BIT reg8Bit)
        :IInstructions(cycles),
         _8BitReg(reg8Bit){};

    void doInstruction(IMemory& memory) override {
        uint8_t regAValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint8_t valueToAdd = memory.get8BitRegister(_8BitReg);
        uint8_t carryValue = 0x00;
        if (memory.isSetFlag(IMemory::FLAG::C)) {
            carryValue = 0x01;
        }
        uint8_t result = regAValue + valueToAdd + carryValue;
        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        if ((((regAValue & 0x0F) + (valueToAdd & 0x0F) + (carryValue & 0x0F)) & 0x10) == 0x10) {
            memory.setFlag(IMemory::FLAG::H);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::H);
        }
        if (((static_cast<uint16_t>(regAValue) + static_cast<uint16_t>(valueToAdd) + carryValue) > 0x00ff)) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        memory.unsetFlag(IMemory::FLAG::N);

        memory.set8BitRegister(IMemory::REG8BIT::A, result);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }
    IMemory::REG8BIT _8BitReg;
};

// 0x8E
class ADC_ARR : public IInstructions
{
public:
    ADC_ARR (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint8_t regAValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint16_t adress = memory.get16BitRegister(IMemory::REG16BIT::HL);
        uint8_t valueToAdd = memory.readInMemory(adress);

        uint8_t carryValue = 0x00;
        if (memory.isSetFlag(IMemory::FLAG::C)) {
            carryValue = 0x01;
        }
        uint8_t result = regAValue + valueToAdd + carryValue;
        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        if ((((regAValue & 0x0F) + (valueToAdd & 0x0F) + (carryValue & 0x0F)) & 0x10) == 0x10) {
            memory.setFlag(IMemory::FLAG::H);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::H);
        }
        if (((static_cast<uint16_t>(regAValue) + static_cast<uint16_t>(valueToAdd) + carryValue) > 0x00ff)) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        memory.unsetFlag(IMemory::FLAG::N);

        memory.set8BitRegister(IMemory::REG8BIT::A, result);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 1);
    }
};

// 0xCE
class ADC_N : public IInstructions
{
public:
    ADC_N (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint8_t regAValue = memory.get8BitRegister(IMemory::REG8BIT::A);
        uint16_t cursor = memory.get16BitRegister(IMemory::REG16BIT::PC);
        uint8_t valueToAdd = memory.readInMemory(cursor + 1);

        uint8_t carryValue = 0x00;
        if (memory.isSetFlag(IMemory::FLAG::C)) {
            carryValue = 0x01;
        }
        uint8_t result = regAValue + valueToAdd + carryValue;
        if (result == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        if ((((regAValue & 0x0F) + (valueToAdd & 0x0F) + (carryValue & 0x0F)) & 0x10) == 0x10) {
            memory.setFlag(IMemory::FLAG::H);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::H);
        }
        if (((static_cast<uint16_t>(regAValue) + static_cast<uint16_t>(valueToAdd) + carryValue) > 0x00ff)) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        memory.unsetFlag(IMemory::FLAG::N);

        memory.set8BitRegister(IMemory::REG8BIT::A, result);
        memory.set16BitRegister(IMemory::REG16BIT::PC, cursor + 2);
    }
};
#endif /*INSTRUCTIONS*/
