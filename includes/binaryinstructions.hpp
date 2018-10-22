#ifndef _BINARYINSTRUCTIONS_
#define _BINARYINSTRUCTIONS_

#include <iostream>
#include <bitset>
#include "iinstructions.hpp"

class SET : public IInstructions
{
public:
    SET (int cycles, int bit, IMemory::REG8BIT reg)
        :IInstructions(cycles),
         _bit(bit),
         _reg8Bit(reg){};

    void doInstruction(IMemory& memory) override {
        memory.setBitInRegister(_bit, _reg8Bit);
    }

private:
    int _bit;
    IMemory::REG8BIT _reg8Bit;
};

class SET_ARR : public IInstructions
{
public:
    SET_ARR (int cycles, int bit)
        :IInstructions(cycles),
         _bit(bit){};

    void doInstruction(IMemory& memory) override {
        uint16_t adress = memory.get16BitRegister(IMemory::REG16BIT::HL);
        uint8_t value = memory.readInMemory(adress);
        std::bitset<8> bitsetValue(value);
        bitsetValue[_bit] = 1;
        uint8_t newValue = static_cast<uint8_t>(bitsetValue.to_ulong());
        memory.writeInMemory(newValue, adress);
    }

private:
    int _bit;
};

class RES : public IInstructions
{
public:
    RES (int cycles, int bit, IMemory::REG8BIT reg)
        :IInstructions(cycles),
         _bit(bit),
         _reg8Bit(reg){};

    void doInstruction(IMemory& memory) override {
        memory.unsetBitInRegister(_bit, _reg8Bit);
    }

private:
    int _bit;
    IMemory::REG8BIT _reg8Bit;
};

class RES_ARR : public IInstructions
{
public:
    RES_ARR (int cycles, int bit)
        :IInstructions(cycles),
         _bit(bit){};

    void doInstruction(IMemory& memory) override {
        uint16_t adress = memory.get16BitRegister(IMemory::REG16BIT::HL);
        uint8_t value = memory.readInMemory(adress);
        std::bitset<8> bitsetValue(value);
        bitsetValue[_bit] = 0;
        uint8_t newValue = static_cast<uint8_t>(bitsetValue.to_ulong());
        memory.writeInMemory(newValue, adress);
    }

private:
    int _bit;
};

class BIT : public IInstructions
{
public:
    BIT (int cycles, int bit, IMemory::REG8BIT reg)
        :IInstructions(cycles),
         _bit(bit),
         _reg8Bit(reg){};

    void doInstruction(IMemory& memory) override {
        if (memory.isSet(_bit, _reg8Bit)) {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        else {
            memory.setFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::N);
        memory.setFlag(IMemory::FLAG::H);
    }

private:
    int _bit;
    IMemory::REG8BIT _reg8Bit;
};

class BIT_ARR : public IInstructions
{
public:
    BIT_ARR (int cycles, int bit)
        :IInstructions(cycles),
         _bit(bit){};

    void doInstruction(IMemory& memory) override {
        uint16_t adress = memory.get16BitRegister(IMemory::REG16BIT::HL);
        uint8_t value = memory.readInMemory(adress);
        std::bitset<8> bitsetValue(value);
        if (bitsetValue[_bit] == 0) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::N);
        memory.setFlag(IMemory::FLAG::H);
    }

private:
    int _bit;
};

class RLC : public IInstructions
{
public:
    RLC (int cycles, IMemory::REG8BIT reg)
        :IInstructions(cycles),
         _reg8Bit(reg){};

    void doInstruction(IMemory& memory) override {
        uint8_t regValue = memory.get8BitRegister(_reg8Bit);
        std::bitset<8> bitsetToRotate(regValue);

        bool isSet = bitsetToRotate[7];
        bitsetToRotate = bitsetToRotate << 1;
        if(isSet) {
            bitsetToRotate[0] = 1;
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        uint8_t rotatedValue = static_cast<uint8_t>(bitsetToRotate.to_ulong());

        if (rotatedValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);

        memory.set8BitRegister(_reg8Bit, rotatedValue);
    }

private:
    IMemory::REG8BIT _reg8Bit;
};

class RLC_ARR : public IInstructions
{
public:
    RLC_ARR (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint16_t adress = memory.get16BitRegister(IMemory::REG16BIT::HL);
        uint8_t regValue = memory.readInMemory(adress);
        std::bitset<8> bitsetToRotate(regValue);

        bool isSet = bitsetToRotate[7];
        bitsetToRotate = bitsetToRotate << 1;
        if(isSet) {
            bitsetToRotate[0] = 1;
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        uint8_t rotatedValue = static_cast<uint8_t>(bitsetToRotate.to_ulong());

        if (rotatedValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);

        memory.writeInMemory(rotatedValue, adress);
    }
};

class RRC : public IInstructions
{
public:
    RRC (int cycles, IMemory::REG8BIT reg)
        :IInstructions(cycles),
         _reg8Bit(reg){};

    void doInstruction(IMemory& memory) override {
        uint8_t regValue = memory.get8BitRegister(_reg8Bit);
        std::bitset<8> bitsetToRotate(regValue);

        bool isSet = bitsetToRotate[0];
        bitsetToRotate = bitsetToRotate >> 1;
        if(isSet) {
            bitsetToRotate[7] = 1;
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        uint8_t rotatedValue = static_cast<uint8_t>(bitsetToRotate.to_ulong());

        if (rotatedValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);

        memory.set8BitRegister(_reg8Bit, rotatedValue);
    }

private:
    IMemory::REG8BIT _reg8Bit;
};

class RRC_ARR : public IInstructions
{
public:
    RRC_ARR (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint16_t adress = memory.get16BitRegister(IMemory::REG16BIT::HL);
        uint8_t regValue = memory.readInMemory(adress);
        std::bitset<8> bitsetToRotate(regValue);

        bool isSet = bitsetToRotate[0];
        bitsetToRotate = bitsetToRotate >> 1;
        if(isSet) {
            bitsetToRotate[7] = 1;
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        uint8_t rotatedValue = static_cast<uint8_t>(bitsetToRotate.to_ulong());

        if (rotatedValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);

        memory.writeInMemory(rotatedValue, adress);
    }
};

class RL : public IInstructions
{
public:
    RL (int cycles, IMemory::REG8BIT reg)
        :IInstructions(cycles),
         _reg8Bit(reg){};

    void doInstruction(IMemory& memory) override {
        uint8_t regValue = memory.get8BitRegister(_reg8Bit);
        std::bitset<8> bitsetToRotate(regValue);

        bool isSet = bitsetToRotate[7];
        bitsetToRotate = bitsetToRotate << 1;
        int carryValue = memory.isSetFlag(IMemory::FLAG::C);
        bitsetToRotate[0] = carryValue;
        if(isSet) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        uint8_t rotatedValue = static_cast<uint8_t>(bitsetToRotate.to_ulong());

        if (rotatedValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);

        memory.set8BitRegister(_reg8Bit, rotatedValue);
    }

private:
    IMemory::REG8BIT _reg8Bit;
};

class RL_ARR : public IInstructions
{
public:
    RL_ARR (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint16_t adress = memory.get16BitRegister(IMemory::REG16BIT::HL);
        uint8_t regValue = memory.readInMemory(adress);
        std::bitset<8> bitsetToRotate(regValue);

        bool isSet = bitsetToRotate[7];
        bitsetToRotate = bitsetToRotate << 1;
        int carryValue = memory.isSetFlag(IMemory::FLAG::C);
        bitsetToRotate[0] = carryValue;
        if(isSet) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        uint8_t rotatedValue = static_cast<uint8_t>(bitsetToRotate.to_ulong());

        if (rotatedValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);

        memory.writeInMemory(rotatedValue, adress);
    }
};

class RR : public IInstructions
{
public:
    RR (int cycles, IMemory::REG8BIT reg)
        :IInstructions(cycles),
         _reg8Bit(reg){};

    void doInstruction(IMemory& memory) override {
        uint8_t regValue = memory.get8BitRegister(_reg8Bit);
        std::bitset<8> bitsetToRotate(regValue);

        bool isSet = bitsetToRotate[0];
        bitsetToRotate = bitsetToRotate >> 1;
        int carryValue = memory.isSetFlag(IMemory::FLAG::C);
        bitsetToRotate[7] = carryValue;
        if(isSet) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        uint8_t rotatedValue = static_cast<uint8_t>(bitsetToRotate.to_ulong());

        if (rotatedValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);

        memory.set8BitRegister(_reg8Bit, rotatedValue);
    }

private:
    IMemory::REG8BIT _reg8Bit;
};

class RR_ARR : public IInstructions
{
public:
    RR_ARR (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint16_t adress = memory.get16BitRegister(IMemory::REG16BIT::HL);
        uint8_t regValue = memory.readInMemory(adress);
        std::bitset<8> bitsetToRotate(regValue);

        bool isSet = bitsetToRotate[0];
        bitsetToRotate = bitsetToRotate >> 1;
        int carryValue = memory.isSetFlag(IMemory::FLAG::C);
        bitsetToRotate[7] = carryValue;
        if(isSet) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        uint8_t rotatedValue = static_cast<uint8_t>(bitsetToRotate.to_ulong());

        if (rotatedValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);

        memory.writeInMemory(rotatedValue, adress);
    }
};

class SLA : public IInstructions
{
public:
    SLA (int cycles, IMemory::REG8BIT reg)
        :IInstructions(cycles),
         _reg8Bit(reg){};

    void doInstruction(IMemory& memory) override {
        uint8_t regValue = memory.get8BitRegister(_reg8Bit);
        std::bitset<8> bitsetToRotate(regValue);

        bool isSet = bitsetToRotate[7];
        bitsetToRotate = bitsetToRotate << 1;
        bitsetToRotate[0] = 0;
        if(isSet) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        uint8_t rotatedValue = static_cast<uint8_t>(bitsetToRotate.to_ulong());

        if (rotatedValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);

        memory.set8BitRegister(_reg8Bit, rotatedValue);
    }

private:
    IMemory::REG8BIT _reg8Bit;
};

class SLA_ARR : public IInstructions
{
public:
    SLA_ARR (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint16_t adress = memory.get16BitRegister(IMemory::REG16BIT::HL);
        uint8_t regValue = memory.readInMemory(adress);
        std::bitset<8> bitsetToRotate(regValue);

        bool isSet = bitsetToRotate[7];
        bitsetToRotate = bitsetToRotate << 1;
        bitsetToRotate[0] = 0;
        if(isSet) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        uint8_t rotatedValue = static_cast<uint8_t>(bitsetToRotate.to_ulong());

        if (rotatedValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);

        memory.writeInMemory(rotatedValue, adress);
    }
};

class SRA : public IInstructions
{
public:
    SRA (int cycles, IMemory::REG8BIT reg)
        :IInstructions(cycles),
         _reg8Bit(reg){};

    void doInstruction(IMemory& memory) override {
        uint8_t regValue = memory.get8BitRegister(_reg8Bit);
        std::bitset<8> bitsetToRotate(regValue);

        bool isSet = bitsetToRotate[0];
        bool carryValue = bitsetToRotate[7];
        bitsetToRotate = bitsetToRotate >> 1;
        bitsetToRotate[7] = carryValue;
        if(isSet) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        uint8_t rotatedValue = static_cast<uint8_t>(bitsetToRotate.to_ulong());

        if (rotatedValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);

        memory.set8BitRegister(_reg8Bit, rotatedValue);
    }

private:
    IMemory::REG8BIT _reg8Bit;
};

class SRA_ARR : public IInstructions
{
public:
    SRA_ARR (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint16_t adress = memory.get16BitRegister(IMemory::REG16BIT::HL);
        uint8_t regValue = memory.readInMemory(adress);
        std::bitset<8> bitsetToRotate(regValue);

        bool isSet = bitsetToRotate[0];
        bool carryValue = bitsetToRotate[7];
        bitsetToRotate = bitsetToRotate >> 1;
        bitsetToRotate[7] = carryValue;
        if(isSet) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        uint8_t rotatedValue = static_cast<uint8_t>(bitsetToRotate.to_ulong());

        if (rotatedValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);

        memory.writeInMemory(rotatedValue, adress);
    }
};

class SRL : public IInstructions
{
public:
    SRL (int cycles, IMemory::REG8BIT reg)
        :IInstructions(cycles),
         _reg8Bit(reg){};

    void doInstruction(IMemory& memory) override {
        uint8_t regValue = memory.get8BitRegister(_reg8Bit);
        std::bitset<8> bitsetToRotate(regValue);

        bool isSet = bitsetToRotate[0];
        bitsetToRotate = bitsetToRotate >> 1;
        bitsetToRotate[7] = 0;
        if(isSet) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        uint8_t rotatedValue = static_cast<uint8_t>(bitsetToRotate.to_ulong());

        if (rotatedValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);

        memory.set8BitRegister(_reg8Bit, rotatedValue);
    }

private:
    IMemory::REG8BIT _reg8Bit;
};

class SRL_ARR : public IInstructions
{
public:
    SRL_ARR (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint16_t adress = memory.get16BitRegister(IMemory::REG16BIT::HL);
        uint8_t regValue = memory.readInMemory(adress);
        std::bitset<8> bitsetToRotate(regValue);

        bool isSet = bitsetToRotate[0];
        bitsetToRotate = bitsetToRotate >> 1;
        bitsetToRotate[7] = 0;
        if(isSet) {
            memory.setFlag(IMemory::FLAG::C);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::C);
        }
        uint8_t rotatedValue = static_cast<uint8_t>(bitsetToRotate.to_ulong());

        if (rotatedValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);

        memory.writeInMemory(rotatedValue, adress);
    }
};

class SWAP : public IInstructions
{
public:
    SWAP (int cycles, IMemory::REG8BIT reg)
        :IInstructions(cycles),
         _reg8Bit(reg){};

    void doInstruction(IMemory& memory) override {
        uint8_t regValue = memory.get8BitRegister(_reg8Bit);
        uint8_t newValue =(regValue << 4) | (regValue >> 4);

        if (newValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);
        memory.unsetFlag(IMemory::FLAG::C);

        memory.set8BitRegister(_reg8Bit, newValue);
    }

private:
    IMemory::REG8BIT _reg8Bit;
};

class SWAP_ARR : public IInstructions
{
public:
    SWAP_ARR (int cycles)
        :IInstructions(cycles){};

    void doInstruction(IMemory& memory) override {
        uint16_t adress = memory.get16BitRegister(IMemory::REG16BIT::HL);
        uint8_t regValue = memory.readInMemory(adress);
        uint8_t newValue =(regValue << 4) | (regValue >> 4);

        if (newValue == 0x00) {
            memory.setFlag(IMemory::FLAG::Z);
        }
        else {
            memory.unsetFlag(IMemory::FLAG::Z);
        }
        memory.unsetFlag(IMemory::FLAG::H);
        memory.unsetFlag(IMemory::FLAG::N);
        memory.unsetFlag(IMemory::FLAG::C);

        memory.writeInMemory(newValue, adress);
    }
};
#endif /*BINARYINSTRUCTIONS*/
