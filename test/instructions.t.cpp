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
    MOCK_METHOD2(set8BitRegister, void(IMemory::REG8BIT, uint8_t));
    MOCK_METHOD2(set16BitRegister, void(IMemory::REG16BIT, uint16_t));
    MOCK_METHOD1(get8BitRegister, uint8_t(IMemory::REG8BIT));
    MOCK_METHOD1(get16BitRegister, uint16_t(IMemory::REG16BIT));
    Registers _registers;

};

class InstructionTest : public ::testing::Test
{
public:

    InstructionTest()
    {
        _cartridge.fill(0);
        _memory._registers.pc = 0;
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

    EXPECT_CALL(_memory, getReadOnlyMemory())
        .WillOnce(Return(getDataForTest(opCode)));

    EXPECT_EQ((int)_memory.get16BitRegister(IMemory::REG16BIT::PC), 0x0000);
    instructions.load16BitToSP(_memory);
    EXPECT_EQ((int)_memory.get16BitRegister(IMemory::REG16BIT::PC), 0x0003);
    EXPECT_EQ((int)_memory.get16BitRegister(IMemory::REG16BIT::SP), 0xfffe);
}
