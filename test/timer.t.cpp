#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "timer.hpp"


using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;

class MockMemory : public IMemory
{
public:

    MOCK_METHOD0(incrementDividerRegister, void());
    MOCK_METHOD0(incrementScanline, void());
    MOCK_METHOD0(getCartridge, CartridgeData const());
    MOCK_METHOD0(getReadOnlyMemory, RomData const());
    MOCK_METHOD1(setCartridge, bool(CartridgeData const &));
    MOCK_METHOD0(getState, IMemory::State());
    MOCK_METHOD0(initializeMemory, void());
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

class MockInterruptHandler : public IInterruptHandler
{
public:

    MOCK_METHOD0(doInterrupt, void());
    MOCK_METHOD0(isMasterSwitchEnabled, bool());
    MOCK_METHOD0(enableMasterSwitch, void());
    MOCK_METHOD0(disableMasterSwitch, void());
    MOCK_METHOD1(requestInterrupt,void(IInterruptHandler::INTERRUPT));
};

class TimerTest : public ::testing::Test
{
    public:

        MockMemory _memory;
        MockInterruptHandler _interruptHandler;
    };

TEST_F(TimerTest, updateTimerWithClockEnabledAndWithoutOverflow)
{
    Timer timer(_memory, _interruptHandler);
    EXPECT_CALL(_memory, readInMemory(0xff07))
        .WillOnce(Return(0x04)); //clock enabled

    timer.update(4);

}

TEST_F(TimerTest, updateTimerWithClockDisabled)
{
    Timer timer(_memory, _interruptHandler);
    EXPECT_CALL(_memory, readInMemory(0xff07))
        .WillOnce(Return(0x00)); //clock disabled
    EXPECT_CALL(_memory, incrementDividerRegister());
    timer.update(1024);
}

TEST_F(TimerTest, updateTimerWithClockEnabledAndWithOverflow)
{
    std::vector<uint8_t> sequence = {0x04, 0x00};
    Timer timer(_memory, _interruptHandler);
    EXPECT_CALL(_memory, readInMemory(0xff07))
        .WillOnce(Return(0x04)) //clock enabled
        .WillOnce(Return(0x00));//for set frequency
    EXPECT_CALL(_memory, incrementDividerRegister());
    EXPECT_CALL(_memory, readInMemory(0xff05))
        .WillOnce(Return(0xff));
    EXPECT_CALL(_memory, readInMemory(0xff06))
        .WillOnce(Return(0x0f));
    EXPECT_CALL(_memory, writeInMemory(0x0f, 0xff05));
    EXPECT_CALL(_interruptHandler, requestInterrupt(IInterruptHandler::INTERRUPT::TIMER));

    timer.update(1024);
}

TEST_F(TimerTest, IsTimerEnabled)
{
    Timer timer(_memory, _interruptHandler);
    EXPECT_CALL(_memory, readInMemory(0xff07))
        .WillOnce(Return(0x00)) //case 2nd bit not set
        .WillOnce(Return(0x04)); //case 2nd bit set


    EXPECT_FALSE(timer.isOn());
    EXPECT_TRUE(timer.isOn());
}

TEST_F(TimerTest, getClockFrequency)
{
    Timer timer(_memory, _interruptHandler);
    {
        EXPECT_CALL(_memory, readInMemory(0xff07))
            .WillOnce(Return(0x00));
        EXPECT_EQ(4096, timer.getClockFrequency());
    }
    {
        EXPECT_CALL(_memory, readInMemory(0xff07))
            .WillOnce(Return(0x01));
        EXPECT_EQ(262144, timer.getClockFrequency());
    }
    {
        EXPECT_CALL(_memory, readInMemory(0xff07))
            .WillOnce(Return(0x02));
        EXPECT_EQ(65536, timer.getClockFrequency());
    }
    {
        EXPECT_CALL(_memory, readInMemory(0xff07))
            .WillOnce(Return(0x03));
        EXPECT_EQ(16384, timer.getClockFrequency());
    }
}

TEST_F(TimerTest, setClockFrequency)
{
    Timer timer(_memory, _interruptHandler);
    {
        EXPECT_CALL(_memory, readInMemory(0xff07))
            .WillOnce(Return(0x00));
        EXPECT_EQ(1024, timer.setClockFrequency());
    }
    {
        EXPECT_CALL(_memory, readInMemory(0xff07))
            .WillOnce(Return(0x01));
        EXPECT_EQ(16, timer.setClockFrequency());
    }
    {
        EXPECT_CALL(_memory, readInMemory(0xff07))
            .WillOnce(Return(0x02));
        EXPECT_EQ(64, timer.setClockFrequency());
    }
    {
        EXPECT_CALL(_memory, readInMemory(0xff07))
            .WillOnce(Return(0x03));
        EXPECT_EQ(256, timer.setClockFrequency());
    }
}
