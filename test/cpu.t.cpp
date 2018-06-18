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


class MockMemory : public IMemory
{
public:

    MOCK_METHOD0(getCartridge, CartridgeData const());
    MOCK_METHOD0(getReadOnlyMemory, RomData const());
    MOCK_METHOD1(setCartridge, bool(CartridgeData const &));
    MOCK_METHOD2(writeInROM, bool(uint8_t, uint16_t));
    MOCK_METHOD1(readInMemory, uint8_t(uint16_t));
    MOCK_METHOD2(set8BitRegister, void(IMemory::REG8BIT, uint8_t));
    MOCK_METHOD2(set16BitRegister, void(IMemory::REG16BIT, uint16_t));
    MOCK_METHOD1(get8BitRegister, uint8_t(IMemory::REG8BIT));
    MOCK_METHOD1(get16BitRegister, uint16_t(IMemory::REG16BIT));
    MOCK_METHOD1(setFlag, void(IMemory::FLAG));
    MOCK_METHOD1(unsetFlag, void(IMemory::FLAG));
    MOCK_METHOD1(isSetFlag, bool(IMemory::FLAG));
};

class CpuTest : public ::testing::Test
{
public:

    CpuTest() {
        uint8_t hex = 0;
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
    std::string _fileName = "./roms/Pokemon_Bleue.gb";

    IMemory::CartridgeData _cartridge;
};

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

