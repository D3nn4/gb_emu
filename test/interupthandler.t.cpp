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

class InterruptHandlerTest : public ::testing::Test
{
public:

    InterruptHandlerTest() {
        _cartridge.fill(0);
    }

    MockMemory _memory;
    IMemory::CartridgeData _cartridge;


};

TEST_F (InterruptHandlerTest, masterEnableSwitch)
{
    InterruptHandler interruptHandler(_memory);

    EXPECT_FALSE(interruptHandler.isMasterSwitchEnabled());
    interruptHandler.enableMasterSwitch();
    EXPECT_TRUE(interruptHandler.isMasterSwitchEnabled());
    interruptHandler.disableMasterSwitch();
    EXPECT_FALSE(interruptHandler.isMasterSwitchEnabled());
}

TEST_F (InterruptHandlerTest, requestInterrupt)
{
    InterruptHandler interruptHandler(_memory);
    {
        EXPECT_CALL(_memory, readInMemory(0xff0f))
            .WillOnce(Return(0x00));
        EXPECT_CALL(_memory, writeInMemory(0x01, 0xff0f));
        interruptHandler.requestInterrupt(IInterruptHandler::INTERRUPT::VBLANC);
    }
    {
        EXPECT_CALL(_memory, readInMemory(0xff0f))
            .WillOnce(Return(0x00));
        EXPECT_CALL(_memory, writeInMemory(0x02, 0xff0f));
        interruptHandler.requestInterrupt(IInterruptHandler::INTERRUPT::LCD);
    }
    {
        EXPECT_CALL(_memory, readInMemory(0xff0f))
            .WillOnce(Return(0x00));
        EXPECT_CALL(_memory, writeInMemory(0x04, 0xff0f));
        interruptHandler.requestInterrupt(IInterruptHandler::INTERRUPT::TIMER);
    }
    {
        EXPECT_CALL(_memory, readInMemory(0xff0f))
            .WillOnce(Return(0x00));
        EXPECT_CALL(_memory, writeInMemory(0x10, 0xff0f));
        interruptHandler.requestInterrupt(IInterruptHandler::INTERRUPT::JOYPAD);
    }
}

TEST_F (InterruptHandlerTest, doInterruptWhenNoInterrupt)
{
    InterruptHandler interruptHandler(_memory);
    interruptHandler.enableMasterSwitch();
    {
        EXPECT_CALL(_memory, readInMemory(0xff0f))
            .WillOnce(Return(0x00)); // no interrupts requests
        EXPECT_CALL(_memory, readInMemory(0xffff))
            .WillOnce(Return(0x17)); // 0x17 == 00010111 => all interrupts enabled
        interruptHandler.doInterrupt();
        EXPECT_TRUE(interruptHandler.isMasterSwitchEnabled());
    }
    {
        EXPECT_CALL(_memory, readInMemory(0xff0f))
            .WillOnce(Return(0x00)); // no interrupts requests
        EXPECT_CALL(_memory, readInMemory(0xffff))
            .WillOnce(Return(0x00)); // all interrupts disabled
        interruptHandler.doInterrupt();
        EXPECT_TRUE(interruptHandler.isMasterSwitchEnabled());
    }
}

TEST_F (InterruptHandlerTest, doInterruptWhenInterruptNotEnabled)
{
    InterruptHandler interruptHandler(_memory);
    interruptHandler.enableMasterSwitch();
    {
        EXPECT_CALL(_memory, readInMemory(0xff0f))
            .WillOnce(Return(0x01)); //request vBlanc interrupt
        EXPECT_CALL(_memory, readInMemory(0xffff))
            .WillOnce(Return(0x10));
        interruptHandler.doInterrupt();
        EXPECT_TRUE(interruptHandler.isMasterSwitchEnabled());
    }
    {
        EXPECT_CALL(_memory, readInMemory(0xff0f))
            .WillOnce(Return(0x02)); //request lcd interrupt
        EXPECT_CALL(_memory, readInMemory(0xffff))
            .WillOnce(Return(0x01));
        interruptHandler.doInterrupt();
        EXPECT_TRUE(interruptHandler.isMasterSwitchEnabled());
    }
    {
        EXPECT_CALL(_memory, readInMemory(0xff0f))
            .WillOnce(Return(0x04)); //request timer interrupt
        EXPECT_CALL(_memory, readInMemory(0xffff))
            .WillOnce(Return(0x10));
        interruptHandler.doInterrupt();
        EXPECT_TRUE(interruptHandler.isMasterSwitchEnabled());
    }
    {
        EXPECT_CALL(_memory, readInMemory(0xff0f))
            .WillOnce(Return(0x10)); //request joypad interrupt
        EXPECT_CALL(_memory, readInMemory(0xffff))
            .WillOnce(Return(0x01));
        interruptHandler.doInterrupt();
        EXPECT_TRUE(interruptHandler.isMasterSwitchEnabled());
    }
}

TEST_F (InterruptHandlerTest, doInterruptWhenInterruptEnabled)
{
    auto test = [this](uint8_t interruptRequest, uint8_t interruptEnabled, uint16_t serviceRoutineAdress)
                {
                    InterruptHandler interruptHandler(_memory);
                    interruptHandler.enableMasterSwitch();
                    EXPECT_CALL(_memory, readInMemory(0xff0f))
                        .WillOnce(Return(interruptRequest));
                    EXPECT_CALL(_memory, readInMemory(0xffff))
                        .WillOnce(Return(interruptEnabled));
                    EXPECT_CALL(_memory, writeInMemory(0x00, 0xff0f));
                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0100));
                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
                        .WillOnce(Return(0xfffe));
                    EXPECT_CALL(_memory, writeInMemory(0x01, 0xfffe));
                    EXPECT_CALL(_memory, writeInMemory(0x00, 0xfffd));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::SP, 0xfffc));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, serviceRoutineAdress));
                    interruptHandler.doInterrupt();
                    EXPECT_FALSE(interruptHandler.isMasterSwitchEnabled());
                };

    test(0x01, 0x01, 0x0040); //request vBlanc interrupt
    test(0x02, 0x02, 0x0048); //request lcd interrupt
    test(0x04, 0x04, 0x0050); //request timer interrupt
    test(0x10, 0x10, 0x0060); //request joypad interrupt
}
