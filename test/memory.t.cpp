#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>

#include "memory.hpp"
#include "iromloader.hpp"
#include "ifileio.hpp"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArrayArgument;

class MockFileIo : public IFileIO
{
public:

    MOCK_METHOD1(openFile, int(std::string const &));
    MOCK_METHOD2(readFile, int(uint8_t*, int));
    MOCK_METHOD1(closeFile, void(int));
};

class MockRomLoader : public IRomLoader
{
public:

    MOCK_METHOD1(load, bool(std::string const&));
    MOCK_METHOD0(getData, std::vector<uint8_t>());
};

class MemoryTest : public ::testing::Test
{
public:

    MemoryTest() {
        uint8_t hex = 0;
        for (size_t index = 0x0; index < IMemory::cartridgeSize; index++) {
            _emptyCartridge[index] = 0x0;
            _cartridge[index] = hex;
            if (index < 0x4000) {
                _bank0[index] = hex;
            }
            if (hex == 255) {
                hex = 0;
            }
            else {
                hex++;
            }
        }
    }

    MockFileIo _fileIO;
    MockRomLoader _romLoader;

    IMemory::CartridgeData _cartridge;
    IMemory::CartridgeData _emptyCartridge;
    std::array<uint8_t, IMemory::bank0Size> _bank0;
};

TEST_F (MemoryTest, addNewValidCartridge)
{
    Memory mem;
    EXPECT_TRUE(mem.setCartridge(_cartridge));

    IMemory::CartridgeData cartridge = mem.getCartridge();
    for (size_t i = 0; i < cartridge.size(); i++) {
        EXPECT_EQ(_cartridge[i], cartridge[i]);
    }

    IMemory::RomData rom = mem.getReadOnlyMemory();
    for (size_t i = 0; i < _bank0.size(); i++) {
        EXPECT_EQ(_bank0[i], rom[i]);
    }
}

TEST_F (MemoryTest, addNewEmptyCartridge)
{
    Memory mem;
    EXPECT_FALSE(mem.setCartridge(_emptyCartridge));
}
