#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>

#include "memory.hpp"
#include "instructionhandler.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;

class MockMemory : public IMemory
{
public:

    MOCK_METHOD0(getCartridge, CartridgeData const());
    MOCK_METHOD0(getReadOnlyMemory, RomData const());
    MOCK_METHOD1(setCartridge, bool(CartridgeData const &));
    MOCK_METHOD2(writeInROM, bool(uint8_t, uint16_t));
    MOCK_METHOD2(set8BitRegister, void(IMemory::REG8BIT, uint8_t));
    MOCK_METHOD2(set16BitRegister, void(IMemory::REG16BIT, uint16_t));
    MOCK_METHOD1(get8BitRegister, uint8_t(IMemory::REG8BIT));
    MOCK_METHOD1(get16BitRegister, uint16_t(IMemory::REG16BIT));
    MOCK_METHOD2(setBitInRegister, void(int, IMemory::REG8BIT));
    MOCK_METHOD2(unsetBitInRegister, void(int, IMemory::REG8BIT));
    MOCK_METHOD2(setBitInRegister, void(int, IMemory::REG16BIT));
    MOCK_METHOD2(unsetBitInRegister, void(int, IMemory::REG16BIT));
    MOCK_METHOD2(isSet, bool(int, IMemory::REG8BIT));
    MOCK_METHOD2(isSet, bool(int, IMemory::REG16BIT));
};

class InstructionHandlerTest : public ::testing::Test
{
public:

    InstructionHandlerTest() {
        _cartridge.fill(0);
    }

    MockMemory _memory;

    IMemory::CartridgeData _cartridge;

    int load16NextBitToRegister(IMemory::RomData rom, IMemory::REG16BIT reg);
    int load8BitRegValueToAdressAt16BitReg(IMemory::RomData rom, IMemory::REG8BIT reg8Bit, IMemory::REG16BIT reg16Bit, int toAdd);
    int load8BitRegValueTo8BitReg(uint8_t opcode, IMemory::REG8BIT toCopyTo, IMemory::REG8BIT toCopyFrom);
    int increment8BitRegister(uint8_t opCode, IMemory::REG8BIT reg);
    int increment16BitRegister(uint16_t opCode, IMemory::REG16BIT reg);

};

template <class ARRAY>
IMemory::RomData getDataForTest(ARRAY opCode)
{
    IMemory::RomData readOnlyMemory;
    readOnlyMemory.fill(0);
    std::copy(opCode.begin(),
              opCode.begin() + opCode.size(),
              readOnlyMemory.begin());
    return readOnlyMemory;
}

int InstructionHandlerTest::load16NextBitToRegister(IMemory::RomData rom, IMemory::REG16BIT reg)
{
    InstructionHandler instructionHandler(_memory);

    uint16_t next16Bit = ((uint16_t)rom[2] << 8) | rom[1];

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, getReadOnlyMemory())
        .WillOnce(Return(rom));
    EXPECT_CALL(_memory, set16BitRegister(reg ,next16Bit));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC ,0x0003));

    return instructionHandler.doInstruction(rom[0]);
}

int InstructionHandlerTest::load8BitRegValueToAdressAt16BitReg(IMemory::RomData rom, IMemory::REG8BIT reg8Bit, IMemory::REG16BIT reg16Bit, int toAdd)
{
    InstructionHandler instructionHandler(_memory);

    EXPECT_CALL(_memory, get8BitRegister(reg8Bit))
        .WillOnce(Return(0xff));
    EXPECT_CALL(_memory, get16BitRegister(reg16Bit))
        .WillOnce(Return(0x8000));
    EXPECT_CALL(_memory, writeInROM(0xff, 0x8000))
        .WillOnce(Return(true));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
    if(toAdd != 0) {
        EXPECT_CALL(_memory, set16BitRegister(reg16Bit, 0x8000 + toAdd));
    }

    return instructionHandler.doInstruction(rom[0]);
}

int InstructionHandlerTest::load8BitRegValueTo8BitReg(uint8_t opCode, IMemory::REG8BIT toCopyTo, IMemory::REG8BIT toCopyFrom)
{
    InstructionHandler instructionHandler(_memory);

    EXPECT_CALL(_memory, get8BitRegister(toCopyFrom))
        .WillOnce(Return(0xff));
    EXPECT_CALL(_memory, set8BitRegister(toCopyTo, 0xff));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

    return instructionHandler.doInstruction(opCode);
}

int InstructionHandlerTest::increment8BitRegister(uint8_t opCode, IMemory::REG8BIT reg)
{
    InstructionHandler instructionHandler(_memory);

    EXPECT_CALL(_memory, get8BitRegister(reg))
        .WillOnce(Return(0x00));
    EXPECT_CALL(_memory, set8BitRegister(reg, 0x01));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
    return instructionHandler.doInstruction(opCode);
}

int InstructionHandlerTest::increment16BitRegister(uint16_t opCode, IMemory::REG16BIT reg)
{
    InstructionHandler instructionHandler(_memory);

    EXPECT_CALL(_memory, get16BitRegister(reg))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(reg, 0x0001));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
    return instructionHandler.doInstruction(opCode);
}

//OpCode 0x00
TEST_F (InstructionHandlerTest, instructionNop)
{
    InstructionHandler instructionHandler(_memory);
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

    EXPECT_EQ(4, instructionHandler.doInstruction(0x00));
}

//OpCode 0x01 0x11 0x21 0x31
TEST_F(InstructionHandlerTest, load16NextBitToRegisterInstructions)
{
    std::array<uint8_t, 3> opCodeAnd16NextBit = {0x01, 0xfe, 0xff};
    EXPECT_EQ(12, load16NextBitToRegister(getDataForTest(opCodeAnd16NextBit), IMemory::REG16BIT::BC));

    opCodeAnd16NextBit = {0x11, 0xfe, 0xff};
    EXPECT_EQ(12, load16NextBitToRegister(getDataForTest(opCodeAnd16NextBit), IMemory::REG16BIT::DE));

    opCodeAnd16NextBit = {0x21, 0xfe, 0xff};
    EXPECT_EQ(12, load16NextBitToRegister(getDataForTest(opCodeAnd16NextBit), IMemory::REG16BIT::HL));

    opCodeAnd16NextBit = {0x31, 0xfe, 0xff};
    EXPECT_EQ(12, load16NextBitToRegister(getDataForTest(opCodeAnd16NextBit), IMemory::REG16BIT::SP));
}

//OpCode 0x02 0x12 0x22 0x32 0x70 to 0x75 0x77
TEST_F (InstructionHandlerTest, load8BitRegValueToAdressAt16BitRegInstructions)
{
    auto testLD = [this](uint8_t opCode, IMemory::REG8BIT reg8Bit, IMemory::REG16BIT reg16Bit, int toAdd)
    {
       std::array<uint8_t, 1> data = {opCode};
       EXPECT_EQ(8, load8BitRegValueToAdressAt16BitReg(getDataForTest(data),reg8Bit, reg16Bit, toAdd));
    };
    testLD(0x02, IMemory::REG8BIT::A, IMemory::REG16BIT::BC, 0);
    testLD(0x12, IMemory::REG8BIT::A, IMemory::REG16BIT::DE, 0);
    testLD(0x22, IMemory::REG8BIT::A, IMemory::REG16BIT::HL, 1);
    testLD(0x32, IMemory::REG8BIT::A, IMemory::REG16BIT::HL, -1);
    testLD(0x70, IMemory::REG8BIT::B, IMemory::REG16BIT::HL, 0);
    testLD(0x71, IMemory::REG8BIT::C, IMemory::REG16BIT::HL, 0);
    testLD(0x72, IMemory::REG8BIT::D, IMemory::REG16BIT::HL, 0);
    testLD(0x73, IMemory::REG8BIT::E, IMemory::REG16BIT::HL, 0);
    testLD(0x74, IMemory::REG8BIT::H, IMemory::REG16BIT::HL, 0);
    testLD(0x75, IMemory::REG8BIT::L, IMemory::REG16BIT::HL, 0);
    testLD(0x77, IMemory::REG8BIT::A, IMemory::REG16BIT::HL, 0);
}

//OpCode 0x03 0x04 0x0C 0x13 0x14 0x1C 0x23 0x24 0x2C 0x33 0x3C
TEST_F (InstructionHandlerTest, incrementRegisterInstructions)
{
    auto testIncr8Bit = [this](uint8_t opCode, IMemory::REG8BIT reg8Bit)
                  {
                      EXPECT_EQ(4, increment8BitRegister(opCode, reg8Bit));
                  };
    testIncr8Bit(0x04, IMemory::REG8BIT::B);
    testIncr8Bit(0x0C, IMemory::REG8BIT::C);
    testIncr8Bit(0x14, IMemory::REG8BIT::D);
    testIncr8Bit(0x1C, IMemory::REG8BIT::E);
    testIncr8Bit(0x24, IMemory::REG8BIT::H);
    testIncr8Bit(0x2C, IMemory::REG8BIT::L);
    testIncr8Bit(0x3C, IMemory::REG8BIT::A);

    auto testIncr16Bit = [this](uint8_t opCode, IMemory::REG16BIT reg16Bit)
                        {
                            EXPECT_EQ(8, increment16BitRegister(opCode, reg16Bit));
                        };
    testIncr16Bit(0x03, IMemory::REG16BIT::BC);
    testIncr16Bit(0x13, IMemory::REG16BIT::DE);
    testIncr16Bit(0x23, IMemory::REG16BIT::HL);
    testIncr16Bit(0x33, IMemory::REG16BIT::SP);
}


//OpCode 0x40 to 0x45 0x47 to 0x4D 0x4F 0x50 to 0x55 0x57 to 0x5D 0x5F 0x60 to 0x65 0x67 to 0x6D 0x6F 0x78 to 0x7D 0x7F
TEST_F (InstructionHandlerTest, load8BitValueTo8BitRegInstructions)
{
auto testLD = [this](uint8_t opCode, IMemory::REG8BIT toCopyTo, IMemory::REG8BIT toCopyFrom)
              {
                  EXPECT_EQ(4, load8BitRegValueTo8BitReg(opCode,toCopyTo, toCopyFrom));
              };
 testLD(0x40, IMemory::REG8BIT::B, IMemory::REG8BIT::B);
 testLD(0x41, IMemory::REG8BIT::B, IMemory::REG8BIT::C);
 testLD(0x42, IMemory::REG8BIT::B, IMemory::REG8BIT::D);
 testLD(0x43, IMemory::REG8BIT::B, IMemory::REG8BIT::E);
 testLD(0x44, IMemory::REG8BIT::B, IMemory::REG8BIT::H);
 testLD(0x45, IMemory::REG8BIT::B, IMemory::REG8BIT::L);
 testLD(0x47, IMemory::REG8BIT::B, IMemory::REG8BIT::A);
 testLD(0x48, IMemory::REG8BIT::C, IMemory::REG8BIT::B);
 testLD(0x49, IMemory::REG8BIT::C, IMemory::REG8BIT::C);
 testLD(0x4A, IMemory::REG8BIT::C, IMemory::REG8BIT::D);
 testLD(0x4B, IMemory::REG8BIT::C, IMemory::REG8BIT::E);
 testLD(0x4C, IMemory::REG8BIT::C, IMemory::REG8BIT::H);
 testLD(0x4D, IMemory::REG8BIT::C, IMemory::REG8BIT::L);
 testLD(0x4F, IMemory::REG8BIT::C, IMemory::REG8BIT::A);
 testLD(0x50, IMemory::REG8BIT::D, IMemory::REG8BIT::B);
 testLD(0x51, IMemory::REG8BIT::D, IMemory::REG8BIT::C);
 testLD(0x52, IMemory::REG8BIT::D, IMemory::REG8BIT::D);
 testLD(0x53, IMemory::REG8BIT::D, IMemory::REG8BIT::E);
 testLD(0x54, IMemory::REG8BIT::D, IMemory::REG8BIT::H);
 testLD(0x55, IMemory::REG8BIT::D, IMemory::REG8BIT::L);
 testLD(0x57, IMemory::REG8BIT::D, IMemory::REG8BIT::A);
 testLD(0x58, IMemory::REG8BIT::E, IMemory::REG8BIT::B);
 testLD(0x59, IMemory::REG8BIT::E, IMemory::REG8BIT::C);
 testLD(0x5A, IMemory::REG8BIT::E, IMemory::REG8BIT::D);
 testLD(0x5B, IMemory::REG8BIT::E, IMemory::REG8BIT::E);
 testLD(0x5C, IMemory::REG8BIT::E, IMemory::REG8BIT::H);
 testLD(0x5D, IMemory::REG8BIT::E, IMemory::REG8BIT::L);
 testLD(0x5F, IMemory::REG8BIT::E, IMemory::REG8BIT::A);
 testLD(0x60, IMemory::REG8BIT::H, IMemory::REG8BIT::B);
 testLD(0x61, IMemory::REG8BIT::H, IMemory::REG8BIT::C);
 testLD(0x62, IMemory::REG8BIT::H, IMemory::REG8BIT::D);
 testLD(0x63, IMemory::REG8BIT::H, IMemory::REG8BIT::E);
 testLD(0x64, IMemory::REG8BIT::H, IMemory::REG8BIT::H);
 testLD(0x65, IMemory::REG8BIT::H, IMemory::REG8BIT::L);
 testLD(0x67, IMemory::REG8BIT::H, IMemory::REG8BIT::A);
 testLD(0x68, IMemory::REG8BIT::L, IMemory::REG8BIT::B);
 testLD(0x69, IMemory::REG8BIT::L, IMemory::REG8BIT::C);
 testLD(0x6A, IMemory::REG8BIT::L, IMemory::REG8BIT::D);
 testLD(0x6B, IMemory::REG8BIT::L, IMemory::REG8BIT::E);
 testLD(0x6C, IMemory::REG8BIT::L, IMemory::REG8BIT::H);
 testLD(0x6D, IMemory::REG8BIT::L, IMemory::REG8BIT::L);
 testLD(0x6F, IMemory::REG8BIT::L, IMemory::REG8BIT::A);
 testLD(0x78, IMemory::REG8BIT::A, IMemory::REG8BIT::B);
 testLD(0x79, IMemory::REG8BIT::A, IMemory::REG8BIT::C);
 testLD(0x7A, IMemory::REG8BIT::A, IMemory::REG8BIT::D);
 testLD(0x7B, IMemory::REG8BIT::A, IMemory::REG8BIT::E);
 testLD(0x7C, IMemory::REG8BIT::A, IMemory::REG8BIT::H);
 testLD(0x7D, IMemory::REG8BIT::A, IMemory::REG8BIT::L);
 testLD(0x7F, IMemory::REG8BIT::A, IMemory::REG8BIT::A);
}
