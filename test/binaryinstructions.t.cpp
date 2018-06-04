#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>

#include "imemory.hpp"
#include "memory.hpp"
#include "binaryinstructions.hpp"

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

class BinaryInstructionTest : public ::testing::Test
{
public:

    MockMemory _memory;

};

TEST_F(BinaryInstructionTest, checkBit7InHWhenIsSetThenUnsetZFlag)
{
    BinaryInstructions bInstructions;

    EXPECT_CALL(_memory, isSet(7, IMemory::REG8BIT::H))
        .WillOnce(Return(true));
    EXPECT_CALL(_memory, unsetBitInRegister((int)IMemory::FLAG::Z, IMemory::REG8BIT::F));

    bInstructions._instructions[0x7c].handler(_memory);

}

TEST_F(BinaryInstructionTest, checkBit7InHWhenIsNotSetThenSetZFlag)
{
    BinaryInstructions bInstructions;

    EXPECT_CALL(_memory, isSet(7, IMemory::REG8BIT::H))
        .WillOnce(Return(false));
    EXPECT_CALL(_memory, setBitInRegister((int)IMemory::FLAG::Z, IMemory::REG8BIT::F));

    bInstructions._instructions[0x7c].handler(_memory);

}
