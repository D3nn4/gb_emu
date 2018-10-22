#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <bitset>

#include "memory.hpp"
#include "interupthandler.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;
using ::testing::InSequence;

class MockMemory : public IMemory
{
public:

    MOCK_METHOD0(getCartridge, CartridgeData const());
    MOCK_METHOD0(getReadOnlyMemory, RomData const());
    MOCK_METHOD1(setCartridge, bool(CartridgeData const &));
    MOCK_METHOD2(writeInMemory, bool(uint8_t, uint16_t));
    MOCK_METHOD1(readInMemory, uint8_t(uint16_t));
    MOCK_METHOD2(set8BitRegister, void(IMemory::REG8BIT, uint8_t));
    MOCK_METHOD2(set16BitRegister, void(IMemory::REG16BIT, uint16_t));
    MOCK_METHOD1(get8BitRegister, uint8_t(IMemory::REG8BIT));
    MOCK_METHOD1(get16BitRegister, uint16_t(IMemory::REG16BIT));
    MOCK_METHOD1(setFlag, void(IMemory::FLAG));
    MOCK_METHOD1(unsetFlag, void(IMemory::FLAG));
    MOCK_METHOD1(isSetFlag, bool(IMemory::FLAG));
    MOCK_METHOD2(setBitInRegister, void(int, IMemory::REG8BIT));
    MOCK_METHOD2(unsetBitInRegister, void(int, IMemory::REG8BIT));
    MOCK_METHOD2(isSet, bool(int, IMemory::REG8BIT));
};

class InteruptHandlerTest : public ::testing::Test
{
public:

    InteruptHandlerTest() {
        _cartridge.fill(0);
    }

    MockMemory _memory;
    IMemory::CartridgeData _cartridge;


};

TEST_F (InteruptHandlerTest, initClassVar)
{
    InteruptHandler interuptHandler(_memory);

    EXPECT_FALSE(interuptHandler.isInteruptEnable());
}
