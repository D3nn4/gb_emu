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

//OpCode 0x02 0x12 0x22 0x32 0x70 0x71 0x72 0x73 0x74 0x75 0x77
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

//OpCode 0x40
TEST_F (InstructionHandlerTest, load8BitValueTo8BitRegInstructions)
{
auto testLD = [this](uint8_t opCode, IMemory::REG8BIT toCopyTo, IMemory::REG8BIT toCopyFrom)
              {
                  EXPECT_EQ(4, load8BitRegValueTo8BitReg(opCode,toCopyTo, toCopyFrom));
              };
testLD(0x40, IMemory::REG8BIT::B, IMemory::REG8BIT::B);
// testLD(0x12, IMemory::REG8BIT::A, IMemory::REG8BIT::DE);
// testLD(0x22, IMemory::REG8BIT::A, IMemory::REG8BIT::HL);
// testLD(0x32, IMemory::REG8BIT::A, IMemory::REG8BIT::HL);
// testLD(0x70, IMemory::REG8BIT::B, IMemory::REG8BIT::HL);
// testLD(0x71, IMemory::REG8BIT::C, IMemory::REG8BIT::HL);
// testLD(0x72, IMemory::REG8BIT::D, IMemory::REG8BIT::HL);
// testLD(0x73, IMemory::REG8BIT::E, IMemory::REG8BIT::HL);
// testLD(0x74, IMemory::REG8BIT::H, IMemory::REG8BIT::HL);
// testLD(0x75, IMemory::REG8BIT::L, IMemory::REG8BIT::HL);
// testLD(0x77, IMemory::REG8BIT::A, IMemory::REG8BIT::HL);
}
