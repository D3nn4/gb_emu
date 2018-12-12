#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <bitset>

#include "memory.hpp"
#include "instructionhandler.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;
using ::testing::InSequence;

class MockInterruptHandler : public IInterruptHandler
{
public:

    MOCK_METHOD0(doInterrupt, void());
    MOCK_METHOD0(isMasterSwitchEnabled, bool());
    MOCK_METHOD0(enableMasterSwitch, void());
    MOCK_METHOD0(disableMasterSwitch, void());
    MOCK_METHOD1(requestInterrupt, void(IInterruptHandler::INTERRUPT));
};

class MockMemory : public IMemory
{
public:

    MOCK_METHOD0(incrementDivderRegister, void());
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

class InstructionHandlerTest : public ::testing::Test
{
public:

    InstructionHandlerTest() {
        _cartridge.fill(0);
    }

    int load16NextBitToRegister(IMemory::RomData rom, IMemory::REG16BIT reg);
    int load8NextBitToRegister(IMemory::RomData rom, IMemory::REG8BIT reg);
    int load8BitRegValueToAdressAt16BitReg(IMemory::RomData rom, IMemory::REG8BIT reg8Bit, IMemory::REG16BIT reg16Bit, int toAdd);
    int load8BitRegValueTo8BitReg(uint8_t opcode, IMemory::REG8BIT toCopyTo, IMemory::REG8BIT toCopyFrom);
    int addValueTo8BitRegister(uint8_t opCode, IMemory::REG8BIT reg, int value);
    int addValueTo16BitRegister(uint16_t opCode, IMemory::REG16BIT reg, int value);
    int addValueAtAdressInReg(uint16_t opCode, IMemory::REG16BIT reg, int value);
    int loadValueFromAdressIn16BitRegto8BitReg(IMemory::RomData rom, IMemory::REG8BIT reg8Bit, IMemory::REG16BIT reg16Bit, int toAdd);
    int xorAWith8BitReg(int opCode, IMemory::REG8BIT reg8Bit);
    int orAWith8BitReg(int opCode, IMemory::REG8BIT reg8Bit);
    int andAWith8BitReg(int opCode, IMemory::REG8BIT reg8Bit);
    int popIn16BitReg(uint8_t opCode, IMemory::REG16BIT reg16Bit);
    int push16BitReg(uint8_t opCode, IMemory::REG16BIT reg16Bit);
    int add8BitToA(uint8_t opCode, IMemory::REG8BIT toAdd);
    int add16BitToHL(uint8_t opCode, IMemory::REG16BIT toAdd);
    int add8BitAndCarryToA(uint8_t opCode, IMemory::REG8BIT toAdd);
    int sub8BitToA(uint8_t opCode, IMemory::REG8BIT toSub);
    int sub8BitAndCarryToA(uint8_t opCode, IMemory::REG8BIT toSub);
    int cp8BitToA(uint8_t opCode, IMemory::REG8BIT toCp);
    void jumpToNext16BitWithFlagSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet);
    void jumpToNext16BitWithFlagNotSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet);
    void jumpToAdressAndNext8BitWithFlagNotSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet);
    void jumpToAdressAndNext8BitWithFlagSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet);
    void callNext16BitWithFlagNotSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet);
    void callNext16BitWithFlagSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet);
    void returnPCToAdressWithFlagNotSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet);
    void returnPCToAdressWithFlagSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet);
    void restartPCAdress(uint8_t opCode, uint8_t value);

    MockMemory _memory;
    MockInterruptHandler _interruptHandler;
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

//instructions

//// TEST LOAD
TEST_F (InstructionHandlerTest, instructionNop)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

    EXPECT_EQ(4, instructionHandler.doInstruction(0x00));
}

int InstructionHandlerTest::load16NextBitToRegister(IMemory::RomData rom, IMemory::REG16BIT reg)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    uint16_t next16Bit = ((uint16_t)rom[2] << 8) | rom[1];

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0002))
        .WillOnce(Return(rom[2]));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(rom[1]));
    EXPECT_CALL(_memory, set16BitRegister(reg ,next16Bit));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC ,0x0003));

    return instructionHandler.doInstruction(rom[0]);
}

TEST_F (InstructionHandlerTest, load16NextBitToRegisterInstructions)
{
    auto testLD =
        [this](uint8_t opCode, IMemory::REG16BIT reg16Bit)
        {
            std::array<uint8_t, 3> data = {opCode, 0xfe, 0xff};
            EXPECT_EQ(12, load16NextBitToRegister(getDataForTest(data), reg16Bit));
        };
    testLD(0x01, IMemory::REG16BIT::BC);
    testLD(0x11, IMemory::REG16BIT::DE);
    testLD(0x21, IMemory::REG16BIT::HL);
    testLD(0x31, IMemory::REG16BIT::SP);
}

int InstructionHandlerTest::load8NextBitToRegister(IMemory::RomData rom, IMemory::REG8BIT reg)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    uint8_t next8Bit = rom[1];

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(next8Bit));
    EXPECT_CALL(_memory, set8BitRegister(reg ,next8Bit));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC ,0x0002));

    return instructionHandler.doInstruction(rom[0]);
}

TEST_F (InstructionHandlerTest, load8NextBitToRegisterInstructions)
{
    auto testLD =
        [this](uint8_t opCode, IMemory::REG8BIT reg8Bit)
        {
            std::array<uint8_t, 2> data = {opCode, 0xfe};
            EXPECT_EQ(8, load8NextBitToRegister(getDataForTest(data), reg8Bit));
        };
    testLD(0x06, IMemory::REG8BIT::B);
    testLD(0x0E, IMemory::REG8BIT::C);
    testLD(0x16, IMemory::REG8BIT::D);
    testLD(0x1E, IMemory::REG8BIT::E);
    testLD(0x26, IMemory::REG8BIT::H);
    testLD(0x2E, IMemory::REG8BIT::L);
    testLD(0x3E, IMemory::REG8BIT::A);
}

int InstructionHandlerTest::load8BitRegValueToAdressAt16BitReg(IMemory::RomData rom, IMemory::REG8BIT reg8Bit, IMemory::REG16BIT reg16Bit, int toAdd)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get8BitRegister(reg8Bit))
        .WillOnce(Return(0xff));
    EXPECT_CALL(_memory, get16BitRegister(reg16Bit))
        .WillOnce(Return(0x8000));
    EXPECT_CALL(_memory, writeInMemory(0xff, 0x8000))
        .WillOnce(Return(true));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
    if(toAdd != 0) {
        EXPECT_CALL(_memory, set16BitRegister(reg16Bit, 0x8000 + toAdd));
    }

    return instructionHandler.doInstruction(rom[0]);
}

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

int InstructionHandlerTest::loadValueFromAdressIn16BitRegto8BitReg(IMemory::RomData rom, IMemory::REG8BIT reg8Bit, IMemory::REG16BIT reg16Bit, int toAdd)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(reg16Bit))
        .WillOnce(Return(0xff00));
    EXPECT_CALL(_memory, readInMemory(0xff00))
        .WillOnce(Return(0x00));
    EXPECT_CALL(_memory, set8BitRegister(reg8Bit, 0x00));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
    if(toAdd != 0) {
        EXPECT_CALL(_memory, set16BitRegister(reg16Bit, 0xff00 + toAdd));
    }

    return instructionHandler.doInstruction(rom[0]);
}

TEST_F (InstructionHandlerTest, loadValueFromAdressIn16BitRegto8BitRegInstructions)
{
    auto testLD = [this](uint8_t opCode, IMemory::REG8BIT reg8Bit, IMemory::REG16BIT reg16Bit, int toAdd)
                  {
                      std::array<uint8_t, 1> data = {opCode};
                      EXPECT_EQ(8, loadValueFromAdressIn16BitRegto8BitReg(getDataForTest(data),reg8Bit, reg16Bit, toAdd));
                  };
    testLD(0x0A, IMemory::REG8BIT::A, IMemory::REG16BIT::BC, 0);
    testLD(0x1A, IMemory::REG8BIT::A, IMemory::REG16BIT::DE, 0);
    testLD(0x2A, IMemory::REG8BIT::A, IMemory::REG16BIT::HL, 1);
    testLD(0x3A, IMemory::REG8BIT::A, IMemory::REG16BIT::HL, -1);
    testLD(0x46, IMemory::REG8BIT::B, IMemory::REG16BIT::HL, 0);
    testLD(0x4E, IMemory::REG8BIT::C, IMemory::REG16BIT::HL, 0);
    testLD(0x56, IMemory::REG8BIT::D, IMemory::REG16BIT::HL, 0);
    testLD(0x5E, IMemory::REG8BIT::E, IMemory::REG16BIT::HL, 0);
    testLD(0x66, IMemory::REG8BIT::H, IMemory::REG16BIT::HL, 0);
    testLD(0x6E, IMemory::REG8BIT::L, IMemory::REG16BIT::HL, 0);
    testLD(0x7E, IMemory::REG8BIT::A, IMemory::REG16BIT::HL, 0);
}

int InstructionHandlerTest::load8BitRegValueTo8BitReg(uint8_t opCode, IMemory::REG8BIT toCopyTo, IMemory::REG8BIT toCopyFrom)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get8BitRegister(toCopyFrom))
        .WillOnce(Return(0xff));
    EXPECT_CALL(_memory, set8BitRegister(toCopyTo, 0xff));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

    return instructionHandler.doInstruction(opCode);
}

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

TEST_F (InstructionHandlerTest, load8NextBitToAdressIn16BitReg)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    std::array<uint8_t, 3> data = {0x36, 0xff, 0x00};

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .WillOnce(Return(0xff00));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0xff));
    EXPECT_CALL(_memory, writeInMemory(0xff, 0xff00));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));

    EXPECT_EQ(12, instructionHandler.doInstruction(data[0]));
}

TEST_F (InstructionHandlerTest, loadSPToNext16BitAdress)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    std::array<uint8_t, 3> data = {0x08, 0x00, 0xff};

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0002))
        .WillOnce(Return(0xff));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x00));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
        .WillOnce(Return(0x8000));
    EXPECT_CALL(_memory, writeInMemory(0x00, 0xff00))
        .WillOnce(Return(true));
    EXPECT_CALL(_memory, writeInMemory(0x80, 0xff01))
        .WillOnce(Return(true));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0003));

    EXPECT_EQ(20, instructionHandler.doInstruction(data[0]));
}

TEST_F (InstructionHandlerTest, load8BitRegToNext8BitAdress)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
        .WillOnce(Return(0xff));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x01));
    EXPECT_CALL(_memory, writeInMemory(0xff, 0xff01))
        .WillOnce(Return(true));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));

    EXPECT_EQ(12, instructionHandler.doInstruction(0xE0));
}

TEST_F (InstructionHandlerTest, loadNext8BitAdressTo8BitReg)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x01));
    EXPECT_CALL(_memory, readInMemory(0xff01))
        .WillOnce(Return(0x10));
    EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x10));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));

    EXPECT_EQ(12, instructionHandler.doInstruction(0xF0));
}

TEST_F (InstructionHandlerTest, load8BitRegToAdressIn8BitReg)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::C))
        .WillOnce(Return(0x00));
    EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
        .WillOnce(Return(0xff));
    EXPECT_CALL(_memory, writeInMemory(0xff, 0xff00))
        .WillOnce(Return(true));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

    EXPECT_EQ(8, instructionHandler.doInstruction(0xE2));
}

TEST_F (InstructionHandlerTest, loadValueAtAdressIn8BitRegTo8BitReg)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::C))
        .WillOnce(Return(0x00));
    EXPECT_CALL(_memory, readInMemory(0xff00))
        .WillOnce(Return(0xff));
    EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0xff));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

    EXPECT_EQ(8, instructionHandler.doInstruction(0xF2));
}

TEST_F (InstructionHandlerTest, loadHLToSP)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .WillOnce(Return(0x00ff));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::SP, 0x00ff));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

    EXPECT_EQ(8, instructionHandler.doInstruction(0xF9));
}

TEST_F (InstructionHandlerTest, loadAToAdressAtNext16Bit)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    IMemory::RomData rom;
    rom.fill(0xfe);

    EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
        .WillOnce(Return(0xff));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0002))
        .WillOnce(Return(0xff));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0xfe));
    EXPECT_CALL(_memory, writeInMemory(0xff, 0xfffe))
        .WillOnce(Return(true));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0003));

    EXPECT_EQ(16, instructionHandler.doInstruction(0xEA));
}

TEST_F (InstructionHandlerTest, loadFromAdressAtNext16BitToA)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    IMemory::RomData rom;
    rom.fill(0xfe);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0002))
        .WillOnce(Return(0xff));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0xfe));
    EXPECT_CALL(_memory, readInMemory(0xfffe))
        .WillOnce(Return(0x11));
    EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x11));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0003));

    EXPECT_EQ(16, instructionHandler.doInstruction(0xFA));
}

TEST_F (InstructionHandlerTest, load8NextBitAndSPToHL)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    {
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
            .WillOnce(Return(0xFFF8));
        EXPECT_CALL(_memory, readInMemory(0x0001))
            .WillOnce(Return(0x02));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::HL, 0xFFFA));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));

        EXPECT_EQ(12, instructionHandler.doInstruction(0xF8));
    }
    {
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
            .WillOnce(Return(0xFFF8));
        EXPECT_CALL(_memory, readInMemory(0x0001))
            .WillOnce(Return(0xfe));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::HL, 0xFFF6));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));

        EXPECT_EQ(12, instructionHandler.doInstruction(0xF8));
    }
}
//// TEST INC && DEC
int InstructionHandlerTest::addValueTo8BitRegister(uint8_t opCode, IMemory::REG8BIT reg, int value)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get8BitRegister(reg))
        .WillOnce(Return(0x01));
    EXPECT_CALL(_memory, set8BitRegister(reg, 0x01 + value));
    if ((0x01 + value) == 0x00) {
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
    }
    else {
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    }
    if (value < 0) {
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));
    }
    else {
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    }
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
    return instructionHandler.doInstruction(opCode);
}

int InstructionHandlerTest::addValueTo16BitRegister(uint16_t opCode, IMemory::REG16BIT reg, int value)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(reg))
        .WillOnce(Return(0x0000));
    if ((0x0000 + value) == 0) {
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
    }
    else {
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    }
    if (value < 0) {
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));
    }
    else {
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    }
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, set16BitRegister(reg, 0x0000 + value));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
    return instructionHandler.doInstruction(opCode);
}

int InstructionHandlerTest::addValueAtAdressInReg(uint16_t opCode, IMemory::REG16BIT reg, int value)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    IMemory::RomData rom;
    rom.fill(0x01);
    EXPECT_CALL(_memory, get16BitRegister(reg))
        .WillOnce(Return(0xfffe));
    EXPECT_CALL(_memory, readInMemory(0xfffe))
        .WillOnce(Return(rom[0xfffe]));
    if ((0x01 + value) == 0) {
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
    }
    else {
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    }
    if (value < 0) {
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));
    }
    else {
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    }
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, writeInMemory(rom[0xfffe] + value, 0xfffe));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
    return instructionHandler.doInstruction(opCode);
}

TEST_F (InstructionHandlerTest, incrementValueInstructions)
{
    auto testAddValue8Bit =
        [this](uint8_t opCode, IMemory::REG8BIT reg8Bit)
        {
            EXPECT_EQ(4, addValueTo8BitRegister(opCode, reg8Bit, 1));
        };
    testAddValue8Bit(0x04, IMemory::REG8BIT::B);
    testAddValue8Bit(0x0C, IMemory::REG8BIT::C);
    testAddValue8Bit(0x14, IMemory::REG8BIT::D);
    testAddValue8Bit(0x1C, IMemory::REG8BIT::E);
    testAddValue8Bit(0x24, IMemory::REG8BIT::H);
    testAddValue8Bit(0x2C, IMemory::REG8BIT::L);
    testAddValue8Bit(0x3C, IMemory::REG8BIT::A);

    auto testAddValue16Bit =
        [this](uint8_t opCode, IMemory::REG16BIT reg16Bit)
        {
            EXPECT_EQ(8, addValueTo16BitRegister(opCode, reg16Bit, 1));
        };
    testAddValue16Bit(0x03, IMemory::REG16BIT::BC);
    testAddValue16Bit(0x13, IMemory::REG16BIT::DE);
    testAddValue16Bit(0x23, IMemory::REG16BIT::HL);
    testAddValue16Bit(0x33, IMemory::REG16BIT::SP);

    EXPECT_EQ(12, addValueAtAdressInReg(0x34, IMemory::REG16BIT::HL, 1));

}

TEST_F (InstructionHandlerTest, decrementValueInstructions)
{
    auto testAddValue8Bit =
        [this](uint8_t opCode, IMemory::REG8BIT reg8Bit)
        {
            EXPECT_EQ(4, addValueTo8BitRegister(opCode, reg8Bit, -1));
        };
    testAddValue8Bit(0x05, IMemory::REG8BIT::B);
    testAddValue8Bit(0x0D, IMemory::REG8BIT::C);
    testAddValue8Bit(0x15, IMemory::REG8BIT::D);
    testAddValue8Bit(0x1D, IMemory::REG8BIT::E);
    testAddValue8Bit(0x25, IMemory::REG8BIT::H);
    testAddValue8Bit(0x2D, IMemory::REG8BIT::L);
    testAddValue8Bit(0x3D, IMemory::REG8BIT::A);

    auto testAddValue16Bit =
        [this](uint8_t opCode, IMemory::REG16BIT reg16Bit)
        {
            EXPECT_EQ(8, addValueTo16BitRegister(opCode, reg16Bit, -1));
        };
    testAddValue16Bit(0x0B, IMemory::REG16BIT::BC);
    testAddValue16Bit(0x1B, IMemory::REG16BIT::DE);
    testAddValue16Bit(0x2B, IMemory::REG16BIT::HL);
    testAddValue16Bit(0x3B, IMemory::REG16BIT::SP);

    EXPECT_EQ(12, addValueAtAdressInReg(0x35, IMemory::REG16BIT::HL, -1));

}

//// TEST AND && XOR && OR
int InstructionHandlerTest::xorAWith8BitReg(int opCode, IMemory::REG8BIT reg8Bit)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    if(reg8Bit == IMemory::REG8BIT::A) {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x10))
            .WillOnce(Return(0x10));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x00));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
    }
    else {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x10));
        EXPECT_CALL(_memory, get8BitRegister(reg8Bit))
            .WillOnce(Return(0x01));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x11));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    }
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));

    return instructionHandler.doInstruction(opCode);
}

TEST_F(InstructionHandlerTest, xorAWith8BitRegInstructions)
{
    auto testXOR = [this](uint8_t opCode, IMemory::REG8BIT reg8Bit)
                  {
                      EXPECT_EQ(4, xorAWith8BitReg(opCode,reg8Bit));
                  };
    testXOR(0xA8, IMemory::REG8BIT::B);
    testXOR(0xA9, IMemory::REG8BIT::C);
    testXOR(0xAA, IMemory::REG8BIT::D);
    testXOR(0xAB, IMemory::REG8BIT::E);
    testXOR(0xAC, IMemory::REG8BIT::H);
    testXOR(0xAD, IMemory::REG8BIT::L);
    testXOR(0xAF, IMemory::REG8BIT::A);
}

int InstructionHandlerTest::orAWith8BitReg(int opCode, IMemory::REG8BIT reg8Bit)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);
    if (reg8Bit == IMemory::REG8BIT::A) {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x10))
            .WillOnce(Return(0x10));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x10));
    }
    else {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x10));
        EXPECT_CALL(_memory, get8BitRegister(reg8Bit))
            .WillOnce(Return(0x11));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x11));
    }
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));

    return instructionHandler.doInstruction(opCode);
}

TEST_F(InstructionHandlerTest, orAWith8BitRegInstructions)
{
    auto testOR = [this](uint8_t opCode, IMemory::REG8BIT reg8Bit)
                   {
                       EXPECT_EQ(4, orAWith8BitReg(opCode,reg8Bit));
                   };
    testOR(0xB0, IMemory::REG8BIT::B);
    testOR(0xB1, IMemory::REG8BIT::C);
    testOR(0xB2, IMemory::REG8BIT::D);
    testOR(0xB3, IMemory::REG8BIT::E);
    testOR(0xB4, IMemory::REG8BIT::H);
    testOR(0xB5, IMemory::REG8BIT::L);
    testOR(0xB7, IMemory::REG8BIT::A);
}

int InstructionHandlerTest::andAWith8BitReg(int opCode, IMemory::REG8BIT reg8Bit)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);
    if (reg8Bit == IMemory::REG8BIT::A) {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x10))
            .WillOnce(Return(0x10));
    }
    else {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x10));
        EXPECT_CALL(_memory, get8BitRegister(reg8Bit))
            .WillOnce(Return(0x11));
    }
    EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x10));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));


    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));

    return instructionHandler.doInstruction(opCode);
}

TEST_F(InstructionHandlerTest, andAWith8BitRegInstructions)
{
    auto testAND = [this](uint8_t opCode, IMemory::REG8BIT reg8Bit)
                  {
                      EXPECT_EQ(4, andAWith8BitReg(opCode,reg8Bit));
                  };
    testAND(0xA0, IMemory::REG8BIT::B);
    testAND(0xA1, IMemory::REG8BIT::C);
    testAND(0xA2, IMemory::REG8BIT::D);
    testAND(0xA3, IMemory::REG8BIT::E);
    testAND(0xA4, IMemory::REG8BIT::H);
    testAND(0xA5, IMemory::REG8BIT::L);
    testAND(0xA7, IMemory::REG8BIT::A);
}

TEST_F(InstructionHandlerTest, xorAWith8BitAtAdressInHL)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
        .WillOnce(Return(0x10));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .WillOnce(Return(0x00ff));
    EXPECT_CALL(_memory, readInMemory(0x00ff))
        .WillOnce(Return(0x01));
    EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x11));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));

    EXPECT_EQ(8, instructionHandler.doInstruction(0xAE));
}

TEST_F(InstructionHandlerTest, orAWith8BitAtAdressInHL)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);


    EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
        .WillOnce(Return(0x11));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .WillOnce(Return(0x00ff));
    EXPECT_CALL(_memory, readInMemory(0x00ff))
        .WillOnce(Return(0x10));
    EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x11));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));

    EXPECT_EQ(8, instructionHandler.doInstruction(0xB6));
}

TEST_F(InstructionHandlerTest, andAWith8BitAtAdressInHL)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
        .WillOnce(Return(0x11));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .WillOnce(Return(0x00ff));
    EXPECT_CALL(_memory, readInMemory(0x00ff))
        .WillOnce(Return(0x10));
    EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x10));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));

    EXPECT_EQ(8, instructionHandler.doInstruction(0xA6));
}

TEST_F(InstructionHandlerTest, xorAWithNext8Bit)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    IMemory::RomData rom;
    rom.fill(0x01);

    EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
        .WillOnce(Return(0x10));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x01));
    EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x11));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));

    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));

    EXPECT_EQ(8, instructionHandler.doInstruction(0xEE));
}

TEST_F(InstructionHandlerTest, orAWithNext8Bit)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
        .WillOnce(Return(0x10));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x01));
    EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x11));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));

    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));

    EXPECT_EQ(8, instructionHandler.doInstruction(0xF6));
}

TEST_F(InstructionHandlerTest, andAWithNext8Bit)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
        .WillOnce(Return(0x10));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x01));
    EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x00));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));

    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));

    EXPECT_EQ(8, instructionHandler.doInstruction(0xE6));
}

//// TEST POP && PUSH
int InstructionHandlerTest::popIn16BitReg(uint8_t opCode, IMemory::REG16BIT reg16Bit)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
        .WillOnce(Return(0xFFFC));
    EXPECT_CALL(_memory, readInMemory(0xFFFD))
        .WillOnce(Return(0x3C));
    EXPECT_CALL(_memory, readInMemory(0xFFFC))
        .WillOnce(Return(0x5F));
    EXPECT_CALL(_memory, set16BitRegister(reg16Bit, 0x3C5F));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::SP, 0xFFFE));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0001));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));

    return instructionHandler.doInstruction(opCode);
}

TEST_F (InstructionHandlerTest, popIn16BitRegInstructions)
{
    auto testPOP = [this](uint8_t opCode, IMemory::REG16BIT reg16Bit)
                   {
                       EXPECT_EQ(12, popIn16BitReg(opCode,reg16Bit));
                   };
    testPOP(0xC1, IMemory::REG16BIT::BC);
    testPOP(0xD1, IMemory::REG16BIT::DE);
    testPOP(0xE1, IMemory::REG16BIT::HL);
    testPOP(0xF1, IMemory::REG16BIT::AF);
}

int InstructionHandlerTest::push16BitReg(uint8_t opCode, IMemory::REG16BIT reg16Bit)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(reg16Bit))
        .WillOnce(Return(0x3C5F));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
        .WillOnce(Return(0xFFFE));
    EXPECT_CALL(_memory, writeInMemory(0x3C, 0xFFFD));
    EXPECT_CALL(_memory, writeInMemory(0x5F, 0xFFFC));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::SP, 0xFFFC));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

    return instructionHandler.doInstruction(opCode);
}

TEST_F (InstructionHandlerTest, push16BitRegisterInstructions)
{
    auto testPUSH = [this](uint8_t opCode, IMemory::REG16BIT reg16Bit)
                   {
                       EXPECT_EQ(16, push16BitReg(opCode,reg16Bit));
                   };
    testPUSH(0xC5, IMemory::REG16BIT::BC);
    testPUSH(0xD5, IMemory::REG16BIT::DE);
    testPUSH(0xE5, IMemory::REG16BIT::HL);
    testPUSH(0xF5, IMemory::REG16BIT::AF);
}
//// TEST ADD ADC
int InstructionHandlerTest::add8BitToA(uint8_t opCode, IMemory::REG8BIT toAdd)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    if(toAdd == IMemory::REG8BIT::A) {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x02))
            .WillOnce(Return(0x02));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x04));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    }
    else {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x02));
        EXPECT_CALL(_memory, get8BitRegister(toAdd))
            .WillOnce(Return(0xfe));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x00));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
    }
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
    return instructionHandler.doInstruction(opCode);
}

TEST_F (InstructionHandlerTest, add8BitToAInstructions)
{
    auto testADD = [this](uint8_t opCode, IMemory::REG8BIT toAdd)
                    {
                        EXPECT_EQ(4, add8BitToA(opCode, toAdd));
                    };
    testADD(0x80, IMemory::REG8BIT::B);
    testADD(0x81, IMemory::REG8BIT::C);
    testADD(0x82, IMemory::REG8BIT::D);
    testADD(0x83, IMemory::REG8BIT::E);
    testADD(0x84, IMemory::REG8BIT::H);
    testADD(0x85, IMemory::REG8BIT::L);
    testADD(0x87, IMemory::REG8BIT::A);
}

TEST_F (InstructionHandlerTest, add8BitInAdressToA)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x02));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff00))
            .WillOnce(Return(0xf0));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0xf2));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));

        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
        EXPECT_EQ(8, instructionHandler.doInstruction(0x86));
    }
    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x02));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff00))
            .WillOnce(Return(0xfe));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x00));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));

        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
        EXPECT_EQ(8, instructionHandler.doInstruction(0x86));
    }
}

TEST_F (InstructionHandlerTest, add8NextBitToA)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x02));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, readInMemory(0x0001))
            .WillOnce(Return(0xf0));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0xf2));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));

        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
        EXPECT_EQ(8, instructionHandler.doInstruction(0xC6));
    }
    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x02));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff01))
            .WillOnce(Return(0xfe));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x00));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));

        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0xff02));
        EXPECT_EQ(8, instructionHandler.doInstruction(0xC6));
    }
}

int InstructionHandlerTest::add16BitToHL(uint8_t opCode, IMemory::REG16BIT toAdd)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    if(toAdd == IMemory::REG16BIT::HL) {
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
            .WillOnce(Return(0x8A23))
            .WillOnce(Return(0x8A23));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::HL, 0x1446));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
    }
    else {
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
            .WillOnce(Return(0x8A23));
        EXPECT_CALL(_memory, get16BitRegister(toAdd))
            .WillOnce(Return(0x0605));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::HL, 0x9028));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
    }
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
    return instructionHandler.doInstruction(opCode);
}

TEST_F (InstructionHandlerTest, add16BitToHLInstructions)
{
    auto testADD = [this](uint8_t opCode, IMemory::REG16BIT toAdd)
                    {
                        EXPECT_EQ(8, add16BitToHL(opCode, toAdd));
                    };
    testADD(0x09, IMemory::REG16BIT::BC);
    testADD(0x19, IMemory::REG16BIT::DE);
    testADD(0x29, IMemory::REG16BIT::HL);
    testADD(0x39, IMemory::REG16BIT::SP);
}

int InstructionHandlerTest::add8BitAndCarryToA(uint8_t opCode, IMemory::REG8BIT toAdd)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    if(toAdd == IMemory::REG8BIT::A) {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x02))
            .WillOnce(Return(0x02));
        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(false));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x04));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    }
    else {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0xE1));
        EXPECT_CALL(_memory, get8BitRegister(toAdd))
            .WillOnce(Return(0x0F));
        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(true));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0xF1));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
    }
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
    return instructionHandler.doInstruction(opCode);
}

TEST_F (InstructionHandlerTest, add8BitAndCarryToAInstructions)
{
    auto testADC = [this](uint8_t opCode, IMemory::REG8BIT toAdd)
                    {
                        EXPECT_EQ(4, add8BitAndCarryToA(opCode, toAdd));
                    };
    testADC(0x88, IMemory::REG8BIT::B);
    testADC(0x89, IMemory::REG8BIT::C);
    testADC(0x8A, IMemory::REG8BIT::D);
    testADC(0x8B, IMemory::REG8BIT::E);
    testADC(0x8C, IMemory::REG8BIT::H);
    testADC(0x8D, IMemory::REG8BIT::L);
    testADC(0x8F, IMemory::REG8BIT::A);
}

TEST_F (InstructionHandlerTest, add8BitInAdressAndCarryToA)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x02));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff00))
            .WillOnce(Return(0x02));
        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(false));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x04));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0xff01));
        EXPECT_EQ(8, instructionHandler.doInstruction(0x8E)) ;
    }

    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0xE1));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff00))
            .WillOnce(Return(0x1E));
        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(true));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x00));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
        EXPECT_EQ(8, instructionHandler.doInstruction(0x8E)) ;
    }
}

TEST_F (InstructionHandlerTest, add8NextBitAndCarryToA)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x02));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff01))
            .WillOnce(Return(0x02));
        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(false));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x04));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0xff02));
        EXPECT_EQ(8, instructionHandler.doInstruction(0xCE)) ;
    }

    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0xE1));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff01))
            .WillOnce(Return(0x3B));
        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(true));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x1D));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0xff02));
        EXPECT_EQ(8, instructionHandler.doInstruction(0xCE)) ;
    }

}

TEST_F (InstructionHandlerTest, add8NextBitToSP)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
        .WillOnce(Return(0xFFF8));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x02));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::SP, 0xFFFA));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));

    EXPECT_EQ(16, instructionHandler.doInstruction(0xE8));
}
//// TEST SUB SUBC

int InstructionHandlerTest::sub8BitToA(uint8_t opCode, IMemory::REG8BIT toSub)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    if(toSub == IMemory::REG8BIT::A) {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3E))
            .WillOnce(Return(0x3E));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x00));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    }
    else {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3E));
        EXPECT_CALL(_memory, get8BitRegister(toSub))
            .WillOnce(Return(0x40));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0xFE));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    }
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
    return instructionHandler.doInstruction(opCode);
}

TEST_F (InstructionHandlerTest, sub8BitToAInstructions)
{
    auto testSUB = [this](uint8_t opCode, IMemory::REG8BIT toSub)
                    {
                        EXPECT_EQ(4, sub8BitToA(opCode, toSub));
                    };
    testSUB(0x90, IMemory::REG8BIT::B);
    testSUB(0x91, IMemory::REG8BIT::C);
    testSUB(0x92, IMemory::REG8BIT::D);
    testSUB(0x93, IMemory::REG8BIT::E);
    testSUB(0x94, IMemory::REG8BIT::H);
    testSUB(0x95, IMemory::REG8BIT::L);
    testSUB(0x97, IMemory::REG8BIT::A);
}

TEST_F (InstructionHandlerTest, sub8BitInAdressToA)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3E));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff00))
            .WillOnce(Return(0x0F));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x2F));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));

        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
        EXPECT_EQ(8, instructionHandler.doInstruction(0x96));
    }
    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3E));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff00))
            .WillOnce(Return(0x40));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0xFE));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));

        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
        EXPECT_EQ(8, instructionHandler.doInstruction(0x96));
    }
}

TEST_F (InstructionHandlerTest, sub8NextBitToA)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3E));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, readInMemory(0x0001))
            .WillOnce(Return(0x0F));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x2F));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));

        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
        EXPECT_EQ(8, instructionHandler.doInstruction(0xD6));
    }
    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3E));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff01))
            .WillOnce(Return(0x40));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0xFE));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));

        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0xff02));
        EXPECT_EQ(8, instructionHandler.doInstruction(0xD6));
    }
}

int InstructionHandlerTest::sub8BitAndCarryToA(uint8_t opCode, IMemory::REG8BIT toSub)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    if(toSub == IMemory::REG8BIT::A) {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x02))
            .WillOnce(Return(0x02));
        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(false));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x00));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    }
    else {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3B));
        EXPECT_CALL(_memory, get8BitRegister(toSub))
            .WillOnce(Return(0x4F));
        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(true));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0xEB));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
    }
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
    return instructionHandler.doInstruction(opCode);
}

TEST_F (InstructionHandlerTest, sub8BitAndCarryToAInstructions)
{
    auto testSUB = [this](uint8_t opCode, IMemory::REG8BIT toSub)
                    {
                        EXPECT_EQ(4, sub8BitAndCarryToA(opCode, toSub));
                    };
    testSUB(0x98, IMemory::REG8BIT::B);
    testSUB(0x99, IMemory::REG8BIT::C);
    testSUB(0x9A, IMemory::REG8BIT::D);
    testSUB(0x9B, IMemory::REG8BIT::E);
    testSUB(0x9C, IMemory::REG8BIT::H);
    testSUB(0x9D, IMemory::REG8BIT::L);
    testSUB(0x9F, IMemory::REG8BIT::A);
}

TEST_F (InstructionHandlerTest, sub8BitInAdressAndCarryToA)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3B));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff00))
            .WillOnce(Return(0x2B));
        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(false));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x10));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0xff01));
        EXPECT_EQ(8, instructionHandler.doInstruction(0x9E)) ;
    }

    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3B));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff00))
            .WillOnce(Return(0x4F));
        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(true));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0xEB));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
        EXPECT_EQ(8, instructionHandler.doInstruction(0x9E)) ;
    }
}

TEST_F (InstructionHandlerTest, sub8NextBitAndCarryToA)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
        .WillOnce(Return(0x3B));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0xff00));
    EXPECT_CALL(_memory, readInMemory(0xff01))
        .WillOnce(Return(0x3A));
    EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
        .WillOnce(Return(true));
    EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x00));
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0xff02));
    EXPECT_EQ(8, instructionHandler.doInstruction(0xDE)) ;
}

//// TEST CP
int InstructionHandlerTest::cp8BitToA(uint8_t opCode, IMemory::REG8BIT toCp)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    if(toCp == IMemory::REG8BIT::A) {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3C))
            .WillOnce(Return(0x3C));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    }
    else {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3C));
        EXPECT_CALL(_memory, get8BitRegister(toCp))
            .WillOnce(Return(0x40));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    }
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
    return instructionHandler.doInstruction(opCode);
}

TEST_F (InstructionHandlerTest, cp8BitToAInstructions)
{
    auto testCP = [this](uint8_t opCode, IMemory::REG8BIT toCp)
                    {
                        EXPECT_EQ(4, cp8BitToA(opCode, toCp));
                    };
    testCP(0xB8, IMemory::REG8BIT::B);
    testCP(0xB9, IMemory::REG8BIT::C);
    testCP(0xBA, IMemory::REG8BIT::D);
    testCP(0xBB, IMemory::REG8BIT::E);
    testCP(0xBC, IMemory::REG8BIT::H);
    testCP(0xBD, IMemory::REG8BIT::L);
    testCP(0xBF, IMemory::REG8BIT::A);
}

TEST_F (InstructionHandlerTest, cp8BitInAdressToA)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3C));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff00))
            .WillOnce(Return(0x2F));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));

        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
        EXPECT_EQ(8, instructionHandler.doInstruction(0xBE));
    }
    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3C));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff00))
            .WillOnce(Return(0x40));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));

        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));
        EXPECT_EQ(8, instructionHandler.doInstruction(0xBE));
    }
}

TEST_F (InstructionHandlerTest, cp8NextBitToA)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3C));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, readInMemory(0x0001))
            .WillOnce(Return(0x3C));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));

        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
        EXPECT_EQ(8, instructionHandler.doInstruction(0xFE));
    }
    {
        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3C));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0xff00));
        EXPECT_CALL(_memory, readInMemory(0xff01))
            .WillOnce(Return(0x40));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));

        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0xff02));
        EXPECT_EQ(8, instructionHandler.doInstruction(0xFE));
    }
}

// TEST Rotation instructions
TEST_F (InstructionHandlerTest, rotateRegAContentToTheLeftAndStore7thBitTo0thBit)
{
    {
        InstructionHandler instructionHandler(_memory, _interruptHandler);

        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x85));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x0B));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

        EXPECT_EQ(4, instructionHandler.doInstruction(0x07));
    }
    {
        InstructionHandler instructionHandler(_memory, _interruptHandler);

        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x00));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x00));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

        EXPECT_EQ(4, instructionHandler.doInstruction(0x07));
    }
}

TEST_F (InstructionHandlerTest, rotateRegAContentToTheLeftAndStoreCarryIn0thBit)
{
    {
        InstructionHandler instructionHandler(_memory, _interruptHandler);

        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x95));
        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(true));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x2B));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

        EXPECT_EQ(4, instructionHandler.doInstruction(0x17));
    }
    {
        InstructionHandler instructionHandler(_memory, _interruptHandler);

        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x00));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x00));
        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(false));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

        EXPECT_EQ(4, instructionHandler.doInstruction(0x17));
    }
}

TEST_F (InstructionHandlerTest, rotateRegAContentToTheRightAndStore7thBitTo0thBit)
{
    {
        InstructionHandler instructionHandler(_memory, _interruptHandler);

        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x3B));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x9D));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

        EXPECT_EQ(4, instructionHandler.doInstruction(0x0F));
    }
    {
        InstructionHandler instructionHandler(_memory, _interruptHandler);

        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0xFE));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x7F));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

        EXPECT_EQ(4, instructionHandler.doInstruction(0x0F));
    }
}

TEST_F (InstructionHandlerTest, rotateRegAContentToTheRightAndStoreCarryIn0thBit)
{
    {
        InstructionHandler instructionHandler(_memory, _interruptHandler);

        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x81));
        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(false));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x40));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

        EXPECT_EQ(4, instructionHandler.doInstruction(0x1F));
    }
    {
        InstructionHandler instructionHandler(_memory, _interruptHandler);

        EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
            .WillOnce(Return(0x00));
        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(true));
        EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0x80));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0001));

        EXPECT_EQ(4, instructionHandler.doInstruction(0x1F));
    }
}

// TEST Jump instructions 
TEST_F (InstructionHandlerTest, jumpToNext16Bit)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x00));
    EXPECT_CALL(_memory, readInMemory(0x0002))
        .WillOnce(Return(0x80));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x8000));

    EXPECT_EQ(16, instructionHandler.doInstruction(0xC3));
}

void InstructionHandlerTest::jumpToNext16BitWithFlagNotSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, isSetFlag(flag))
        .WillOnce(Return(false));
    if (isToBeSet == false) {
        EXPECT_CALL(_memory, readInMemory(0x0001))
            .WillOnce(Return(0x00));
        EXPECT_CALL(_memory, readInMemory(0x0002))
            .WillOnce(Return(0x80));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x8000));
        EXPECT_EQ(16, instructionHandler.doInstruction(opCode));
    }
    else {
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0003));
        EXPECT_EQ(12, instructionHandler.doInstruction(opCode));
    }
}

void InstructionHandlerTest::jumpToNext16BitWithFlagSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, isSetFlag(flag))
        .WillOnce(Return(true));
    if (isToBeSet == true) {
        EXPECT_CALL(_memory, readInMemory(0x0001))
            .WillOnce(Return(0x00));
        EXPECT_CALL(_memory, readInMemory(0x0002))
            .WillOnce(Return(0x80));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x8000));
        EXPECT_EQ(16, instructionHandler.doInstruction(opCode));
    }
    else {
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0003));
        EXPECT_EQ(12, instructionHandler.doInstruction(opCode));
    }
}

TEST_F (InstructionHandlerTest, jumpToNext16BitWithConditions)
{
    auto testWithFlagSet = [this](uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
                           {
                               jumpToNext16BitWithFlagSet(opCode, flag, isToBeSet);
                           };
    auto testWithFlagNotSet = [this](uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
                              {
                               jumpToNext16BitWithFlagNotSet(opCode, flag, isToBeSet);
                              };
    testWithFlagNotSet(0xC2, IMemory::FLAG::Z, 0);
    testWithFlagNotSet(0xCA, IMemory::FLAG::Z, 1);
    testWithFlagNotSet(0xD2, IMemory::FLAG::C, 0);
    testWithFlagNotSet(0xDA, IMemory::FLAG::C, 1);

    testWithFlagSet(0xC2, IMemory::FLAG::Z, 0);
    testWithFlagSet(0xCA, IMemory::FLAG::Z, 1);
    testWithFlagSet(0xD2, IMemory::FLAG::C, 0);
    testWithFlagSet(0xDA, IMemory::FLAG::C, 1);
}

TEST_F (InstructionHandlerTest, jumpToAdressInReg)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .WillOnce(Return(0x8000));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x8000));

    EXPECT_EQ(4, instructionHandler.doInstruction(0xE9));
}

TEST_F (InstructionHandlerTest, jumpToAdressAndNext8Bit)
{
    {
        InstructionHandler instructionHandler(_memory, _interruptHandler);

        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x8000));
        EXPECT_CALL(_memory, readInMemory(0x8001))
            .WillOnce(Return(0x10));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x8010));

        EXPECT_EQ(12, instructionHandler.doInstruction(0x18));
    }
    {
        InstructionHandler instructionHandler(_memory, _interruptHandler);

        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x8000));
        EXPECT_CALL(_memory, readInMemory(0x8001))
            .WillOnce(Return(0xFE));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x7FFE));

        EXPECT_EQ(12, instructionHandler.doInstruction(0x18));
    }
}

void InstructionHandlerTest::jumpToAdressAndNext8BitWithFlagNotSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, isSetFlag(flag))
        .WillOnce(Return(false));
    if (isToBeSet == false) {
        EXPECT_CALL(_memory, readInMemory(0x0001))
            .WillOnce(Return(0x10));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0010));
        EXPECT_EQ(12, instructionHandler.doInstruction(opCode));
    }
    else {
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
        EXPECT_EQ(8, instructionHandler.doInstruction(opCode));
    }
}

void InstructionHandlerTest::jumpToAdressAndNext8BitWithFlagSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x8000));
    EXPECT_CALL(_memory, isSetFlag(flag))
        .WillOnce(Return(true));
    if (isToBeSet == true) {
        EXPECT_CALL(_memory, readInMemory(0x8001))
            .WillOnce(Return(0xFE));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x7FFE));
        EXPECT_EQ(12, instructionHandler.doInstruction(opCode));
    }
    else {
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x8002));
        EXPECT_EQ(8, instructionHandler.doInstruction(opCode));
    }
}

TEST_F (InstructionHandlerTest, jumpToAdressAndNext8BitWithConditions)
{
    auto testWithFlagSet = [this](uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
                           {
                               jumpToAdressAndNext8BitWithFlagSet(opCode, flag, isToBeSet);
                           };
    auto testWithFlagNotSet = [this](uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
                              {
                                  jumpToAdressAndNext8BitWithFlagNotSet(opCode, flag, isToBeSet);
                              };
    testWithFlagNotSet(0x20, IMemory::FLAG::Z, 0);
    testWithFlagNotSet(0x28, IMemory::FLAG::Z, 1);
    testWithFlagNotSet(0x30, IMemory::FLAG::C, 0);
    testWithFlagNotSet(0x38, IMemory::FLAG::C, 1);

    testWithFlagSet(0x20, IMemory::FLAG::Z, 0);
    testWithFlagSet(0x28, IMemory::FLAG::Z, 1);
    testWithFlagSet(0x30, IMemory::FLAG::C, 0);
    testWithFlagSet(0x38, IMemory::FLAG::C, 1);
}

// TEST Call instructions 
TEST_F (InstructionHandlerTest, callNext16Bit)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x8000));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
        .WillOnce(Return(0xFFFE));

    EXPECT_CALL(_memory, readInMemory(0x8001))
        .WillOnce(Return(0x34));
    EXPECT_CALL(_memory, readInMemory(0x8002))
        .WillOnce(Return(0x12));

    EXPECT_CALL(_memory, writeInMemory(0x80, 0xFFFD))
        .WillOnce(Return(true));
    EXPECT_CALL(_memory, writeInMemory(0x03, 0xFFFC))
        .WillOnce(Return(true));

    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::SP, 0xFFFC));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x1234));

    EXPECT_EQ(24, instructionHandler.doInstruction(0xCD));
}

void InstructionHandlerTest::callNext16BitWithFlagSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x8000));
    EXPECT_CALL(_memory, isSetFlag(flag))
        .WillOnce(Return(true));

    if (isToBeSet == true) {
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
            .WillOnce(Return(0xFFFE));

        EXPECT_CALL(_memory, readInMemory(0x8001))
            .WillOnce(Return(0x34));
        EXPECT_CALL(_memory, readInMemory(0x8002))
            .WillOnce(Return(0x12));

        EXPECT_CALL(_memory, writeInMemory(0x80, 0xFFFD))
            .WillOnce(Return(true));
        EXPECT_CALL(_memory, writeInMemory(0x03, 0xFFFC))
            .WillOnce(Return(true));

        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::SP, 0xFFFC));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x1234));

        EXPECT_EQ(24, instructionHandler.doInstruction(opCode));
    }
    else {
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x8003));
        EXPECT_EQ(12, instructionHandler.doInstruction(opCode));
    }
}

void InstructionHandlerTest::callNext16BitWithFlagNotSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x8000));
    EXPECT_CALL(_memory, isSetFlag(flag))
        .WillOnce(Return(false));

    if (isToBeSet == false) {
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
            .WillOnce(Return(0xFFFE));

        EXPECT_CALL(_memory, readInMemory(0x8001))
            .WillOnce(Return(0x34));
        EXPECT_CALL(_memory, readInMemory(0x8002))
            .WillOnce(Return(0x12));

        EXPECT_CALL(_memory, writeInMemory(0x80, 0xFFFD))
            .WillOnce(Return(true));
        EXPECT_CALL(_memory, writeInMemory(0x03, 0xFFFC))
            .WillOnce(Return(true));

        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::SP, 0xFFFC));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x1234));

        EXPECT_EQ(24, instructionHandler.doInstruction(opCode));
    }
    else {
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x8003));
        EXPECT_EQ(12, instructionHandler.doInstruction(opCode));
    }
}

TEST_F (InstructionHandlerTest, callNext16BitWithCondition)
{
    auto testWithFlagSet = [this](uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
                           {
                               callNext16BitWithFlagSet(opCode, flag, isToBeSet);
                           };
    auto testWithFlagNotSet = [this](uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
                              {
                                  callNext16BitWithFlagNotSet(opCode, flag, isToBeSet);
                              };
    testWithFlagNotSet(0xC4, IMemory::FLAG::Z, 0);
    testWithFlagNotSet(0xCC, IMemory::FLAG::Z, 1);
    testWithFlagNotSet(0xD4, IMemory::FLAG::C, 0);
    testWithFlagNotSet(0xDC, IMemory::FLAG::C, 1);

    testWithFlagSet(0xC4, IMemory::FLAG::Z, 0);
    testWithFlagSet(0xCC, IMemory::FLAG::Z, 1);
    testWithFlagSet(0xD4, IMemory::FLAG::C, 0);
    testWithFlagSet(0xDC, IMemory::FLAG::C, 1);
}

// TEST Return instructions
TEST_F (InstructionHandlerTest, ReturnPCToAdressInStack)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0000))
        .WillOnce(Return(0x03));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x80));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::SP, 0x0002));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x8003));

    EXPECT_EQ(16, instructionHandler.doInstruction(0xC9));
}

void InstructionHandlerTest::returnPCToAdressWithFlagSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, isSetFlag(flag))
        .WillOnce(Return(true));
    if (isToBeSet == true) {
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, readInMemory(0x0000))
            .WillOnce(Return(0x03));
        EXPECT_CALL(_memory, readInMemory(0x0001))
            .WillOnce(Return(0x80));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::SP, 0x0002));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x8003));

        EXPECT_EQ(20, instructionHandler.doInstruction(opCode));
    }
    else {
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x8000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x8001));
        EXPECT_EQ(8, instructionHandler.doInstruction(opCode));
    }
}

void InstructionHandlerTest::returnPCToAdressWithFlagNotSet(uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, isSetFlag(flag))
        .WillOnce(Return(false));
    if (isToBeSet == false) {
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
            .WillOnce(Return(0x0000));
        EXPECT_CALL(_memory, readInMemory(0x0000))
            .WillOnce(Return(0x03));
        EXPECT_CALL(_memory, readInMemory(0x0001))
            .WillOnce(Return(0x80));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::SP, 0x0002));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x8003));

        EXPECT_EQ(20, instructionHandler.doInstruction(opCode));
    }
    else {
        EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
            .WillOnce(Return(0x8000));
        EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x8001));
        EXPECT_EQ(8, instructionHandler.doInstruction(opCode));
    }
}

TEST_F (InstructionHandlerTest, ReturnPCToAdressInStackWithConditions)
{
    auto testWithFlagSet = [this](uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
                           {
                               returnPCToAdressWithFlagSet(opCode, flag, isToBeSet);
                           };
    auto testWithFlagNotSet = [this](uint8_t opCode, IMemory::FLAG flag, bool isToBeSet)
                              {
                                  returnPCToAdressWithFlagNotSet(opCode, flag, isToBeSet);
                              };
    testWithFlagNotSet(0xC0, IMemory::FLAG::Z, 0);
    testWithFlagNotSet(0xC8, IMemory::FLAG::Z, 1);
    testWithFlagNotSet(0xD0, IMemory::FLAG::C, 0);
    testWithFlagNotSet(0xD8, IMemory::FLAG::C, 1);

    testWithFlagSet(0xC0, IMemory::FLAG::Z, 0);
    testWithFlagSet(0xC8, IMemory::FLAG::Z, 1);
    testWithFlagSet(0xD0, IMemory::FLAG::C, 0);
    testWithFlagSet(0xD8, IMemory::FLAG::C, 1);
}

TEST_F (InstructionHandlerTest, ReturnPCToAdressInStackAndEnableInterrupts)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0000))
        .WillOnce(Return(0x03));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x80));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::SP, 0x0002));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x8003));

    EXPECT_CALL(_interruptHandler, enableMasterSwitch());
    EXPECT_EQ(16, instructionHandler.doInstruction(0xD9));
}

// TEST Restart instructions
void InstructionHandlerTest::restartPCAdress(uint8_t opCode, uint8_t value)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x8000));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::SP))
        .WillOnce(Return(0xFFF9));
    EXPECT_CALL(_memory, writeInMemory(0x80, 0xFFF8))
        .WillOnce(Return(true));
    EXPECT_CALL(_memory, writeInMemory(0x01, 0xFFF7))
        .WillOnce(Return(true));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::SP, 0xFFF7));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0000 + value));

    EXPECT_EQ(16, instructionHandler.doInstruction(opCode));
}

TEST_F (InstructionHandlerTest, restartPCAdress)
{
    auto test = [this](uint8_t opCode, uint8_t value)
                              {
                                  restartPCAdress(opCode, value);
                              };
    test(0xC7, 0x00);
    test(0xD7, 0x10);
    test(0xE7, 0x20);
    test(0xF7, 0x30);
    test(0xCF, 0x08);
    test(0xDF, 0x18);
    test(0xEF, 0x28);
    test(0xFF, 0x38);
}

// TEST MISC

TEST_F (InstructionHandlerTest, setCarryFlagInstruction)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));

    EXPECT_EQ(4, instructionHandler.doInstruction(0x37));
}

TEST_F (InstructionHandlerTest, complementCarryFlag)
{
    {
        InstructionHandler instructionHandler(_memory, _interruptHandler);

        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(false));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));

        EXPECT_EQ(4, instructionHandler.doInstruction(0x3F));
    }
    {
        InstructionHandler instructionHandler(_memory, _interruptHandler);

        EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
            .WillOnce(Return(true));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
        EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));

        EXPECT_EQ(4, instructionHandler.doInstruction(0x3F));
    }
}

TEST_F (InstructionHandlerTest, complementRegA)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get8BitRegister(IMemory::REG8BIT::A))
        .WillOnce(Return(0x09));
    EXPECT_CALL(_memory, set8BitRegister(IMemory::REG8BIT::A, 0xF6));
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));

    EXPECT_EQ(4, instructionHandler.doInstruction(0x2F));
}

TEST_F (InstructionHandlerTest, enableDisableInterrups)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_interruptHandler, disableMasterSwitch());
    EXPECT_EQ(4, instructionHandler.doInstruction(0xF3));
    EXPECT_CALL(_interruptHandler, enableMasterSwitch());
    EXPECT_EQ(4, instructionHandler.doInstruction(0xFB));
}

TEST_F (InstructionHandlerTest, halt)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    //TODO

    EXPECT_EQ(4, instructionHandler.doInstruction(0x76));
}

TEST_F (InstructionHandlerTest, stop)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    //TODO

    EXPECT_EQ(4, instructionHandler.doInstruction(0x10));
}


//binaryInstructions

TEST_F (InstructionHandlerTest, rotateLeft8BitRegAndPutInCarryBinaryInstruction)
{
    auto test = [this](uint8_t opCode, IMemory::REG8BIT reg)
                {
                    InstructionHandler instructionHandler(_memory, _interruptHandler);

                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0000));
                    EXPECT_CALL(_memory, readInMemory(0x0001))
                        .WillOnce(Return(opCode));
                    EXPECT_CALL(_memory, get8BitRegister(reg))
                        .WillOnce(Return(0x85));
                    EXPECT_CALL(_memory, set8BitRegister(reg, 0x0B));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
                    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
                    EXPECT_EQ(12, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 8 for binary op
                };
    test(0x00, IMemory::REG8BIT::B);
    test(0x01, IMemory::REG8BIT::C);
    test(0x02, IMemory::REG8BIT::D);
    test(0x03, IMemory::REG8BIT::E);
    test(0x04, IMemory::REG8BIT::H);
    test(0x05, IMemory::REG8BIT::L);
    test(0x07, IMemory::REG8BIT::A);


}

TEST_F (InstructionHandlerTest, rotateLeftAtAdressInHLAndPutInCarryBinaryInstruction)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x06)); //opcode for binaryInstruction
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .WillOnce(Return(0xff00));
    EXPECT_CALL(_memory, readInMemory(0xff00))
        .WillOnce(Return(0x00));
    EXPECT_CALL(_memory, writeInMemory(0x00, 0xff00));
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
    EXPECT_EQ(20, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 16 for binary op
}

TEST_F (InstructionHandlerTest, rotateRight8BitAndPutInCarryRegBinaryInstruction)
{
    auto test = [this](uint8_t opCode, IMemory::REG8BIT reg)
                {
                    InstructionHandler instructionHandler(_memory, _interruptHandler);

                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0000));
                    EXPECT_CALL(_memory, readInMemory(0x0001))
                        .WillOnce(Return(opCode));
                    EXPECT_CALL(_memory, get8BitRegister(reg))
                        .WillOnce(Return(0x01));
                    EXPECT_CALL(_memory, set8BitRegister(reg, 0x80));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
                    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
                    EXPECT_EQ(12, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 8 for binary op
                };
    test(0x08, IMemory::REG8BIT::B);
    test(0x09, IMemory::REG8BIT::C);
    test(0x0A, IMemory::REG8BIT::D);
    test(0x0B, IMemory::REG8BIT::E);
    test(0x0C, IMemory::REG8BIT::H);
    test(0x0D, IMemory::REG8BIT::L);
    test(0x0F, IMemory::REG8BIT::A);


}

TEST_F (InstructionHandlerTest, rotateRightAtAdressInHLAndPutInCarryBinaryInstruction)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x0E)); //opcode for binaryInstruction
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .WillOnce(Return(0xff00));
    EXPECT_CALL(_memory, readInMemory(0xff00))
        .WillOnce(Return(0x00));
    EXPECT_CALL(_memory, writeInMemory(0x00, 0xff00));
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
    EXPECT_EQ(20, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 16 for binary op
}
//

TEST_F (InstructionHandlerTest, rotateLeft8BitRegBinaryInstruction)
{
    auto test = [this](uint8_t opCode, IMemory::REG8BIT reg)
                {
                    InstructionHandler instructionHandler(_memory, _interruptHandler);

                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0000));
                    EXPECT_CALL(_memory, readInMemory(0x0001))
                        .WillOnce(Return(opCode));
                    EXPECT_CALL(_memory, get8BitRegister(reg))
                        .WillOnce(Return(0x80));
                    EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
                        .WillOnce(Return(false));
                    EXPECT_CALL(_memory, set8BitRegister(reg, 0x00));
                    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
                    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
                    EXPECT_EQ(12, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 8 for binary op
                };
    test(0x10, IMemory::REG8BIT::B);
    test(0x11, IMemory::REG8BIT::C);
    test(0x12, IMemory::REG8BIT::D);
    test(0x13, IMemory::REG8BIT::E);
    test(0x14, IMemory::REG8BIT::H);
    test(0x15, IMemory::REG8BIT::L);
    test(0x17, IMemory::REG8BIT::A);
}

TEST_F (InstructionHandlerTest, rotateLeftAtAdressInHLAndBinaryInstruction)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x16)); //opcode for binaryInstruction
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .WillOnce(Return(0xff00));
    EXPECT_CALL(_memory, readInMemory(0xff00))
        .WillOnce(Return(0x11));
    EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
        .WillOnce(Return(false));
    EXPECT_CALL(_memory, writeInMemory(0x22, 0xff00));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
    EXPECT_EQ(20, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 16 for binary op
}

TEST_F (InstructionHandlerTest, rotateRight8BitRegBinaryInstruction)
{
    auto test = [this](uint8_t opCode, IMemory::REG8BIT reg)
                {
                    InstructionHandler instructionHandler(_memory, _interruptHandler);

                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0000));
                    EXPECT_CALL(_memory, readInMemory(0x0001))
                        .WillOnce(Return(opCode));
                    EXPECT_CALL(_memory, get8BitRegister(reg))
                        .WillOnce(Return(0x01));
                    EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
                        .WillOnce(Return(false));
                    EXPECT_CALL(_memory, set8BitRegister(reg, 0x00));
                    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
                    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
                    EXPECT_EQ(12, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 8 for binary op
                };
    test(0x18, IMemory::REG8BIT::B);
    test(0x19, IMemory::REG8BIT::C);
    test(0x1A, IMemory::REG8BIT::D);
    test(0x1B, IMemory::REG8BIT::E);
    test(0x1C, IMemory::REG8BIT::H);
    test(0x1D, IMemory::REG8BIT::L);
    test(0x1F, IMemory::REG8BIT::A);
}

TEST_F (InstructionHandlerTest, rotateRightAtAdressInHLAndBinaryInstruction)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x1E)); //opcode for binaryInstruction
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .WillOnce(Return(0xff00));
    EXPECT_CALL(_memory, readInMemory(0xff00))
        .WillOnce(Return(0x8A));
    EXPECT_CALL(_memory, isSetFlag(IMemory::FLAG::C))
        .WillOnce(Return(false));
    EXPECT_CALL(_memory, writeInMemory(0x45, 0xff00));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
    EXPECT_EQ(20, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 16 for binary op
}

TEST_F (InstructionHandlerTest, shiftLeft8BitRegBinaryInstruction)
{
    auto test = [this](uint8_t opCode, IMemory::REG8BIT reg)
                {
                    InstructionHandler instructionHandler(_memory, _interruptHandler);

                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0000));
                    EXPECT_CALL(_memory, readInMemory(0x0001))
                        .WillOnce(Return(opCode));
                    EXPECT_CALL(_memory, get8BitRegister(reg))
                        .WillOnce(Return(0x80));
                    EXPECT_CALL(_memory, set8BitRegister(reg, 0x00));
                    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
                    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
                    EXPECT_EQ(12, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 8 for binary op
                };
    test(0x20, IMemory::REG8BIT::B);
    test(0x21, IMemory::REG8BIT::C);
    test(0x22, IMemory::REG8BIT::D);
    test(0x23, IMemory::REG8BIT::E);
    test(0x24, IMemory::REG8BIT::H);
    test(0x25, IMemory::REG8BIT::L);
    test(0x27, IMemory::REG8BIT::A);
}

TEST_F (InstructionHandlerTest, shiftLeftAtAdressInHLAndBinaryInstruction)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x26)); //opcode for binaryInstruction
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .WillOnce(Return(0xff00));
    EXPECT_CALL(_memory, readInMemory(0xff00))
        .WillOnce(Return(0xFF));
    EXPECT_CALL(_memory, writeInMemory(0xFE, 0xff00));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
    EXPECT_EQ(20, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 16 for binary op
}

TEST_F (InstructionHandlerTest, shiftRight8BitRegAndRetainBit7BinaryInstruction)
{
    auto test = [this](uint8_t opCode, IMemory::REG8BIT reg)
                {
                    InstructionHandler instructionHandler(_memory, _interruptHandler);

                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0000));
                    EXPECT_CALL(_memory, readInMemory(0x0001))
                        .WillOnce(Return(opCode));
                    EXPECT_CALL(_memory, get8BitRegister(reg))
                        .WillOnce(Return(0x8A));
                    EXPECT_CALL(_memory, set8BitRegister(reg, 0xC5));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
                    EXPECT_EQ(12, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 8 for binary op
                };
    test(0x28, IMemory::REG8BIT::B);
    test(0x29, IMemory::REG8BIT::C);
    test(0x2A, IMemory::REG8BIT::D);
    test(0x2B, IMemory::REG8BIT::E);
    test(0x2C, IMemory::REG8BIT::H);
    test(0x2D, IMemory::REG8BIT::L);
    test(0x2F, IMemory::REG8BIT::A);
}

TEST_F (InstructionHandlerTest, shiftRightAtAdressInHLAndRetainBit7BinaryInstruction)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x2E)); //opcode for binaryInstruction
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .WillOnce(Return(0xff00));
    EXPECT_CALL(_memory, readInMemory(0xff00))
        .WillOnce(Return(0x01));
    EXPECT_CALL(_memory, writeInMemory(0x00, 0xff00));
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
    EXPECT_EQ(20, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 16 for binary op
}

TEST_F (InstructionHandlerTest, swapBinaryInstruction)
{
    auto test = [this](uint8_t opCode, IMemory::REG8BIT reg)
                {
                    InstructionHandler instructionHandler(_memory, _interruptHandler);

                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0000));
                    EXPECT_CALL(_memory, readInMemory(0x0001))
                        .WillOnce(Return(opCode));
                    EXPECT_CALL(_memory, get8BitRegister(reg))
                        .WillOnce(Return(0x8A));
                    EXPECT_CALL(_memory, set8BitRegister(reg, 0xA8));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
                    EXPECT_EQ(12, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 8 for binary op
                };
    test(0x30, IMemory::REG8BIT::B);
    test(0x31, IMemory::REG8BIT::C);
    test(0x32, IMemory::REG8BIT::D);
    test(0x33, IMemory::REG8BIT::E);
    test(0x34, IMemory::REG8BIT::H);
    test(0x35, IMemory::REG8BIT::L);
    test(0x37, IMemory::REG8BIT::A);
}

TEST_F (InstructionHandlerTest, swapInAdressBinaryInstruction)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);
    
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x36));
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .WillOnce(Return(0xFF00));
    EXPECT_CALL(_memory, readInMemory(0xFF00))
        .WillOnce(Return(0xF0));
    EXPECT_CALL(_memory, writeInMemory(0x0F, 0xFF00));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::C));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));

    EXPECT_EQ(20, instructionHandler.doInstruction(0xCB));
};

TEST_F (InstructionHandlerTest, shiftRight8BitRegAndZeroedBit7BinaryInstruction)
{
    auto test = [this](uint8_t opCode, IMemory::REG8BIT reg)
                {
                    InstructionHandler instructionHandler(_memory, _interruptHandler);

                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0000));
                    EXPECT_CALL(_memory, readInMemory(0x0001))
                        .WillOnce(Return(opCode));
                    EXPECT_CALL(_memory, get8BitRegister(reg))
                        .WillOnce(Return(0x01));
                    EXPECT_CALL(_memory, set8BitRegister(reg, 0x00));
                    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
                    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
                    EXPECT_EQ(12, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 8 for binary op
                };
    test(0x38, IMemory::REG8BIT::B);
    test(0x39, IMemory::REG8BIT::C);
    test(0x3A, IMemory::REG8BIT::D);
    test(0x3B, IMemory::REG8BIT::E);
    test(0x3C, IMemory::REG8BIT::H);
    test(0x3D, IMemory::REG8BIT::L);
    test(0x3F, IMemory::REG8BIT::A);
}

TEST_F (InstructionHandlerTest, shiftRightAtAdressInHLAndZeroedBit7BinaryInstruction)
{
    InstructionHandler instructionHandler(_memory, _interruptHandler);

    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
        .WillOnce(Return(0x0000));
    EXPECT_CALL(_memory, readInMemory(0x0001))
        .WillOnce(Return(0x3E)); //opcode for binaryInstruction
    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
        .WillOnce(Return(0xFF00));
    EXPECT_CALL(_memory, readInMemory(0xff00))
        .WillOnce(Return(0xFF));
    EXPECT_CALL(_memory, writeInMemory(0x7F, 0xFF00));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::H));
    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::C));
    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
    EXPECT_EQ(20, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 16 for binary op
}

TEST_F (InstructionHandlerTest, setBitInRegister)
{
    auto test = [this](uint8_t opCode, int bit, IMemory::REG8BIT reg)
                {
                    InstructionHandler instructionHandler(_memory, _interruptHandler);
                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0000));
                    EXPECT_CALL(_memory, readInMemory(0x0001))
                        .WillOnce(Return(opCode));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
                    EXPECT_CALL(_memory, setBitInRegister(bit, reg));
                    EXPECT_EQ(12, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 8 for binary op
                };
    test(0xC0, 0, IMemory::REG8BIT::B);
    test(0xC1, 0, IMemory::REG8BIT::C);
    test(0xC2, 0, IMemory::REG8BIT::D);
    test(0xC3, 0, IMemory::REG8BIT::E);
    test(0xC4, 0, IMemory::REG8BIT::H);
    test(0xC5, 0, IMemory::REG8BIT::L);
    test(0xC7, 0, IMemory::REG8BIT::A);

    test(0xC8, 1, IMemory::REG8BIT::B);
    test(0xC9, 1, IMemory::REG8BIT::C);
    test(0xCA, 1, IMemory::REG8BIT::D);
    test(0xCB, 1, IMemory::REG8BIT::E);
    test(0xCC, 1, IMemory::REG8BIT::H);
    test(0xCD, 1, IMemory::REG8BIT::L);
    test(0xCF, 1, IMemory::REG8BIT::A);

    test(0xD0, 2, IMemory::REG8BIT::B);
    test(0xD1, 2, IMemory::REG8BIT::C);
    test(0xD2, 2, IMemory::REG8BIT::D);
    test(0xD3, 2, IMemory::REG8BIT::E);
    test(0xD4, 2, IMemory::REG8BIT::H);
    test(0xD5, 2, IMemory::REG8BIT::L);
    test(0xD7, 2, IMemory::REG8BIT::A);

    test(0xD8, 3, IMemory::REG8BIT::B);
    test(0xD9, 3, IMemory::REG8BIT::C);
    test(0xDA, 3, IMemory::REG8BIT::D);
    test(0xDB, 3, IMemory::REG8BIT::E);
    test(0xDC, 3, IMemory::REG8BIT::H);
    test(0xDD, 3, IMemory::REG8BIT::L);
    test(0xDF, 3, IMemory::REG8BIT::A);

    test(0xE0, 4, IMemory::REG8BIT::B);
    test(0xE1, 4, IMemory::REG8BIT::C);
    test(0xE2, 4, IMemory::REG8BIT::D);
    test(0xE3, 4, IMemory::REG8BIT::E);
    test(0xE4, 4, IMemory::REG8BIT::H);
    test(0xE5, 4, IMemory::REG8BIT::L);
    test(0xE7, 4, IMemory::REG8BIT::A);

    test(0xE8, 5, IMemory::REG8BIT::B);
    test(0xE9, 5, IMemory::REG8BIT::C);
    test(0xEA, 5, IMemory::REG8BIT::D);
    test(0xEB, 5, IMemory::REG8BIT::E);
    test(0xEC, 5, IMemory::REG8BIT::H);
    test(0xED, 5, IMemory::REG8BIT::L);
    test(0xEF, 5, IMemory::REG8BIT::A);

    test(0xF0, 6, IMemory::REG8BIT::B);
    test(0xF1, 6, IMemory::REG8BIT::C);
    test(0xF2, 6, IMemory::REG8BIT::D);
    test(0xF3, 6, IMemory::REG8BIT::E);
    test(0xF4, 6, IMemory::REG8BIT::H);
    test(0xF5, 6, IMemory::REG8BIT::L);
    test(0xF7, 6, IMemory::REG8BIT::A);

    test(0xF8, 7, IMemory::REG8BIT::B);
    test(0xF9, 7, IMemory::REG8BIT::C);
    test(0xFA, 7, IMemory::REG8BIT::D);
    test(0xFB, 7, IMemory::REG8BIT::E);
    test(0xFC, 7, IMemory::REG8BIT::H);
    test(0xFD, 7, IMemory::REG8BIT::L);
    test(0xFF, 7, IMemory::REG8BIT::A);
}

TEST_F (InstructionHandlerTest, setBitInAdressInHL)
{
    auto test = [this](uint8_t opCode, int bit)
                {
                    std::bitset<8> bitsetToLoad(0x00);
                    bitsetToLoad[bit] = 1;
                    uint8_t newValue = static_cast<uint8_t>(bitsetToLoad.to_ulong());

                    InstructionHandler instructionHandler(_memory, _interruptHandler);

                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0000));
                    EXPECT_CALL(_memory, readInMemory(0x0001))
                        .WillOnce(Return(opCode));
                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
                        .WillOnce(Return(0xff00));
                    EXPECT_CALL(_memory, readInMemory(0xff00))
                        .WillOnce(Return(0x00));
                    EXPECT_CALL(_memory, writeInMemory(newValue, 0xff00))
                        .WillOnce(Return(true));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
                    EXPECT_EQ(20, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 16 for binary op
                };
    test(0xC6, 0);
    test(0xD6, 2);
    test(0xE6, 4);
    test(0xF6, 6);
    test(0xCE, 1);
    test(0xDE, 3);
    test(0xEE, 5);
    test(0xFE, 7);
}

TEST_F (InstructionHandlerTest, resetBitInRegister)
{
    auto test = [this](uint8_t opCode, int bit, IMemory::REG8BIT reg)
                {
                    InstructionHandler instructionHandler(_memory, _interruptHandler);
                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0000));
                    EXPECT_CALL(_memory, readInMemory(0x0001))
                        .WillOnce(Return(opCode));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
                    EXPECT_CALL(_memory, unsetBitInRegister(bit, reg));
                    EXPECT_EQ(12, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 8 for binary op
                };
    test(0x80, 0, IMemory::REG8BIT::B);
    test(0x81, 0, IMemory::REG8BIT::C);
    test(0x82, 0, IMemory::REG8BIT::D);
    test(0x83, 0, IMemory::REG8BIT::E);
    test(0x84, 0, IMemory::REG8BIT::H);
    test(0x85, 0, IMemory::REG8BIT::L);
    test(0x87, 0, IMemory::REG8BIT::A);

    test(0x88, 1, IMemory::REG8BIT::B);
    test(0x89, 1, IMemory::REG8BIT::C);
    test(0x8A, 1, IMemory::REG8BIT::D);
    test(0x8B, 1, IMemory::REG8BIT::E);
    test(0x8C, 1, IMemory::REG8BIT::H);
    test(0x8D, 1, IMemory::REG8BIT::L);
    test(0x8F, 1, IMemory::REG8BIT::A);

    test(0x90, 2, IMemory::REG8BIT::B);
    test(0x91, 2, IMemory::REG8BIT::C);
    test(0x92, 2, IMemory::REG8BIT::D);
    test(0x93, 2, IMemory::REG8BIT::E);
    test(0x94, 2, IMemory::REG8BIT::H);
    test(0x95, 2, IMemory::REG8BIT::L);
    test(0x97, 2, IMemory::REG8BIT::A);

    test(0x98, 3, IMemory::REG8BIT::B);
    test(0x99, 3, IMemory::REG8BIT::C);
    test(0x9A, 3, IMemory::REG8BIT::D);
    test(0x9B, 3, IMemory::REG8BIT::E);
    test(0x9C, 3, IMemory::REG8BIT::H);
    test(0x9D, 3, IMemory::REG8BIT::L);
    test(0x9F, 3, IMemory::REG8BIT::A);

    test(0xA0, 4, IMemory::REG8BIT::B);
    test(0xA1, 4, IMemory::REG8BIT::C);
    test(0xA2, 4, IMemory::REG8BIT::D);
    test(0xA3, 4, IMemory::REG8BIT::E);
    test(0xA4, 4, IMemory::REG8BIT::H);
    test(0xA5, 4, IMemory::REG8BIT::L);
    test(0xA7, 4, IMemory::REG8BIT::A);

    test(0xA8, 5, IMemory::REG8BIT::B);
    test(0xA9, 5, IMemory::REG8BIT::C);
    test(0xAA, 5, IMemory::REG8BIT::D);
    test(0xAB, 5, IMemory::REG8BIT::E);
    test(0xAC, 5, IMemory::REG8BIT::H);
    test(0xAD, 5, IMemory::REG8BIT::L);
    test(0xAF, 5, IMemory::REG8BIT::A);

    test(0xB0, 6, IMemory::REG8BIT::B);
    test(0xB1, 6, IMemory::REG8BIT::C);
    test(0xB2, 6, IMemory::REG8BIT::D);
    test(0xB3, 6, IMemory::REG8BIT::E);
    test(0xB4, 6, IMemory::REG8BIT::H);
    test(0xB5, 6, IMemory::REG8BIT::L);
    test(0xB7, 6, IMemory::REG8BIT::A);

    test(0xB8, 7, IMemory::REG8BIT::B);
    test(0xB9, 7, IMemory::REG8BIT::C);
    test(0xBA, 7, IMemory::REG8BIT::D);
    test(0xBB, 7, IMemory::REG8BIT::E);
    test(0xBC, 7, IMemory::REG8BIT::H);
    test(0xBD, 7, IMemory::REG8BIT::L);
    test(0xBF, 7, IMemory::REG8BIT::A);
}

TEST_F (InstructionHandlerTest, resetBitInAdressInHL)
{
    auto test = [this](uint8_t opCode, int bit)
                {
                    std::bitset<8> bitsetToLoad(0xFF);
                    bitsetToLoad[bit] = 0;
                    uint8_t newValue = static_cast<uint8_t>(bitsetToLoad.to_ulong());

                    InstructionHandler instructionHandler(_memory, _interruptHandler);

                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0000));
                    EXPECT_CALL(_memory, readInMemory(0x0001))
                        .WillOnce(Return(opCode));
                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
                        .WillOnce(Return(0xff00));
                    EXPECT_CALL(_memory, readInMemory(0xff00))
                        .WillOnce(Return(0xFF));
                    EXPECT_CALL(_memory, writeInMemory(newValue, 0xff00))
                        .WillOnce(Return(true));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
                    EXPECT_EQ(20, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 16 for binary op
                };
    test(0x86, 0);
    test(0x96, 2);
    test(0xA6, 4);
    test(0xB6, 6);
    test(0x8E, 1);
    test(0x9E, 3);
    test(0xAE, 5);
    test(0xBE, 7);
}

TEST_F (InstructionHandlerTest, testBitInRegister)
{
    auto test = [this](uint8_t opCode, int bit, IMemory::REG8BIT reg)
                {
                    InstructionHandler instructionHandler(_memory, _interruptHandler);
                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0000));
                    EXPECT_CALL(_memory, readInMemory(0x0001))
                        .WillOnce(Return(opCode));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
                    EXPECT_CALL(_memory, isSet(bit, reg));
                    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::Z));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
                    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
                    EXPECT_EQ(12, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 8 for binary op
                };
    test(0x40, 0, IMemory::REG8BIT::B);
    test(0x41, 0, IMemory::REG8BIT::C);
    test(0x42, 0, IMemory::REG8BIT::D);
    test(0x43, 0, IMemory::REG8BIT::E);
    test(0x44, 0, IMemory::REG8BIT::H);
    test(0x45, 0, IMemory::REG8BIT::L);
    test(0x47, 0, IMemory::REG8BIT::A);

    test(0x48, 1, IMemory::REG8BIT::B);
    test(0x49, 1, IMemory::REG8BIT::C);
    test(0x4A, 1, IMemory::REG8BIT::D);
    test(0x4B, 1, IMemory::REG8BIT::E);
    test(0x4C, 1, IMemory::REG8BIT::H);
    test(0x4D, 1, IMemory::REG8BIT::L);
    test(0x4F, 1, IMemory::REG8BIT::A);

    test(0x50, 2, IMemory::REG8BIT::B);
    test(0x51, 2, IMemory::REG8BIT::C);
    test(0x52, 2, IMemory::REG8BIT::D);
    test(0x53, 2, IMemory::REG8BIT::E);
    test(0x54, 2, IMemory::REG8BIT::H);
    test(0x55, 2, IMemory::REG8BIT::L);
    test(0x57, 2, IMemory::REG8BIT::A);

    test(0x58, 3, IMemory::REG8BIT::B);
    test(0x59, 3, IMemory::REG8BIT::C);
    test(0x5A, 3, IMemory::REG8BIT::D);
    test(0x5B, 3, IMemory::REG8BIT::E);
    test(0x5C, 3, IMemory::REG8BIT::H);
    test(0x5D, 3, IMemory::REG8BIT::L);
    test(0x5F, 3, IMemory::REG8BIT::A);

    test(0x60, 4, IMemory::REG8BIT::B);
    test(0x61, 4, IMemory::REG8BIT::C);
    test(0x62, 4, IMemory::REG8BIT::D);
    test(0x63, 4, IMemory::REG8BIT::E);
    test(0x64, 4, IMemory::REG8BIT::H);
    test(0x65, 4, IMemory::REG8BIT::L);
    test(0x67, 4, IMemory::REG8BIT::A);

    test(0x68, 5, IMemory::REG8BIT::B);
    test(0x69, 5, IMemory::REG8BIT::C);
    test(0x6A, 5, IMemory::REG8BIT::D);
    test(0x6B, 5, IMemory::REG8BIT::E);
    test(0x6C, 5, IMemory::REG8BIT::H);
    test(0x6D, 5, IMemory::REG8BIT::L);
    test(0x6F, 5, IMemory::REG8BIT::A);

    test(0x70, 6, IMemory::REG8BIT::B);
    test(0x71, 6, IMemory::REG8BIT::C);
    test(0x72, 6, IMemory::REG8BIT::D);
    test(0x73, 6, IMemory::REG8BIT::E);
    test(0x74, 6, IMemory::REG8BIT::H);
    test(0x75, 6, IMemory::REG8BIT::L);
    test(0x77, 6, IMemory::REG8BIT::A);

    test(0x78, 7, IMemory::REG8BIT::B);
    test(0x79, 7, IMemory::REG8BIT::C);
    test(0x7A, 7, IMemory::REG8BIT::D);
    test(0x7B, 7, IMemory::REG8BIT::E);
    test(0x7C, 7, IMemory::REG8BIT::H);
    test(0x7D, 7, IMemory::REG8BIT::L);
    test(0x7F, 7, IMemory::REG8BIT::A);
}

TEST_F (InstructionHandlerTest, testBitInAdressInHL)
{
    auto test = [this](uint8_t opCode, int )
                {
                    InstructionHandler instructionHandler(_memory, _interruptHandler);

                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::PC))
                        .WillOnce(Return(0x0000));
                    EXPECT_CALL(_memory, readInMemory(0x0001))
                        .WillOnce(Return(opCode));
                    EXPECT_CALL(_memory, get16BitRegister(IMemory::REG16BIT::HL))
                        .WillOnce(Return(0xff00));
                    EXPECT_CALL(_memory, readInMemory(0xff00))
                        .WillOnce(Return(0xFF));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::Z));
                    EXPECT_CALL(_memory, unsetFlag(IMemory::FLAG::N));
                    EXPECT_CALL(_memory, setFlag(IMemory::FLAG::H));
                    EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, 0x0002));
                    EXPECT_EQ(20, instructionHandler.doInstruction(0xCB));// 4 cycle for opcode CB, 16 for binary op
                };
    test(0x46, 0);
    test(0x56, 2);
    test(0x66, 4);
    test(0x76, 6);
    test(0x4E, 1);
    test(0x5E, 3);
    test(0x6E, 5);
    test(0x7E, 7);
}
