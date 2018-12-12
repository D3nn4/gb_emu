#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>

#include "memory.hpp"
#include "iromloader.hpp"
#include "cpu.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;

class MockRomLoader : public IRomLoader
{
public:

    MOCK_METHOD1(load, bool(std::string const&));
    MOCK_METHOD0(getData, IMemory::CartridgeData());
};

class MockInstructionHandler : public IInstructionHandler
{
public:

    MOCK_METHOD1(doInstruction, int(uint8_t));
};

class MockInstructions : public IInstructions
{
public:
    MockInstructions(int cycles) : IInstructions(cycles){};
    MOCK_METHOD1(doOp, int(IMemory&));
    MOCK_METHOD1(doInstructionImpl, void(IMemory&));

    
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

class CpuTest : public ::testing::Test
{
public:

    CpuTest()
        :_instructions(0)
  {
        uint8_t hex = 0;
        _emptyCartridge.fill(0x00);
        for (size_t index = 0x0; index < IMemory::cartridgeSize; index++) {
            _cartridge[index] = hex;
            if (hex == 255) {
                hex = 0;
            }
            else {
                hex++;
            }
        }
    }

    MockRomLoader _RL;
    MockMemory _memory;
    MockInstructionHandler _instructionHandler;
    MockInstructions _instructions;
    std::string _fileName = "./roms/Pokemon_Bleue.gb";

    IMemory::CartridgeData _cartridge;
    IMemory::CartridgeData _emptyCartridge;
};

// TEST_F (CpuTest, executeOneFrame)
// {
//     CpuTest cpu;
//     uint16_t pcValue = 0x0000;

//     EXPECT_EQ(0, cpu.getCurrentCycles());

//     EXPECT_CALL(_memory, set16BitRegister(IMemory::REG16BIT::PC, pcValue++))
//         .Times(17562);
//     EXPECT_CALL(_memory, readInMemory(pcValue))
//         .WillRepeatedly(Return(0x00));
//     EXPECT_CALL(_instructionHandler, doInstruction(0x00))
//         .Times(17562);
//     EXPECT_CALL(_instructions, doOp(_memory))
//         .Times(17562)
//         .WillRepeatedly(Return(4));
//     EXPECT_CALL(_instructions, doInstruction(_memory))
//         .Times(17562);

//     EXPECT_EQ(0, cpu.getCurrentCycles());

//     EXPECT_EQ(17562, pcValue);
//     cpu.executeOneFrame();
// }

// TEST_F (CpuTest, launchValidGame)
// {
//     EXPECT_CALL(_RL, load(_fileName))
//         .WillRepeatedly(Return(true));
//     EXPECT_CALL(_RL, getData())
//         .WillRepeatedly(Return(_cartridge));
//     // EXPECT_CALL(_memory, setCartridge(_cartridge))
//     //     .WillOnce(Return(true));

//     Cpu cpu(_RL);
//     cpu.launchGame(_fileName);
// }

