#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>

#include "imemory.hpp"
#include "memory.hpp"
#include "instructions.hpp"

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

class InstructionTest : public ::testing::Test
{
public:

    InstructionTest()
    {
        _cartridge.fill(0);
    }

    MockMemory _memory;

    IMemory::CartridgeData _cartridge;
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

TEST_F (InstructionTest, Load16BitInSP)
{
    std::array<uint8_t, 4> opCode = {0x31, 0xfe, 0xff, 0xff};
    Instructions instructions;

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, getReadOnlyMemory())
        .WillOnce(Return(getDataForTest(opCode)));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::SP ,0xfffe));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC ,0x0003));

    // instructions.load16BitToSP(_memory);
    instructions._instructions[opCode[0]].handler(_memory);
}

TEST_F (InstructionTest, xorRegisterA)
{
    std::array<uint8_t, 2> opCode = {0xaf, 0xff};
    Instructions instructions;

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A ,0x00));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC ,0x0001));

    instructions._instructions[opCode[0]].handler(_memory);
}

TEST_F (InstructionTest, Load16BitInHL)
{
    std::array<uint8_t, 4> opCode = {0x21, 0xff, 0x9f, 0xff};
    Instructions instructions;

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, getReadOnlyMemory())
        .WillOnce(Return(getDataForTest(opCode)));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::HL ,0x9fff));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC ,0x0003));

    instructions._instructions[opCode[0]].handler(_memory);
}

TEST_F (InstructionTest, load8BitInRegisterAtAdress)
{
    std::array<uint8_t, 2> opCode = {0x32, 0xff};
    Instructions instructions;

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .Times(2)
        .WillRepeatedly(Return(0x9fff));
    EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
        .WillOnce(Return(0x00));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::HL, 0x9ffe));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC ,0x0001));
    EXPECT_CALL(_memory, writeInROM(0x00, 0x9fff));
    instructions._instructions[opCode[0]].handler(_memory);
}

TEST_F(InstructionTest, BinaryInstruction)
{
    std::array<uint8_t, 3> opCode = {0xCB, 0x7c, 0xff};
    Instructions instructions;

    EXPECT_CALL(_memory, getReadOnlyMemory())
      .WillOnce(Return(getDataForTest(opCode)));
    EXPECT_CALL(_memory, isSet(7, IMemory::REG8BIT::H))
      .WillOnce(Return(false));
    EXPECT_CALL(_memory, setBitInRegister((int)IMemory::FLAG::Z, IMemory::REG8BIT::F));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC ,0x0002));
    instructions._instructions[opCode[0]].handler(_memory);
} 
