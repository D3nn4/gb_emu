#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>

#include "memory.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;

class MockMemory : public IMemory
{
public:

    MOCK_METHOD0(getCartridge, CartridgeData const &());
    MOCK_METHOD0(getReadOnlyMemory, RomData const &());
    MOCK_METHOD1(setCartridge, bool(CartridgeData const &));

    Registers _registers;
};

class InstructionHandlerTest : public ::testing::Test
{
public:


    MockMemory _memory;

};

TEST_F (InstructionHandlerTest, bootRom)
{

}

