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
    MemoryTest();
    MockFileIo _fileIO;
    MockRomLoader _romLoader;
    std::vector<uint8_t> _cartridge;
    std::vector<uint8_t> _emptyCartridge;
    std::vector<uint8_t> _bank0;
private:
};

MemoryTest::MemoryTest()
{
    uint8_t hex = 0;
    for (size_t size = 0x0; size < 0x8000;size++) {
        _cartridge.push_back(hex);
        if (size <= 0x3fff) {
            _bank0.push_back(hex);
        }
        if (hex == 255) {
            hex = 0;
        }
        else {
            hex++;
        }
    }
}
TEST_F (MemoryTest, addNewValidCartridge)
{Memory mem;
    EXPECT_TRUE(mem.setCartridge(_cartridge));
    std::vector<uint8_t> cartridge = mem.getCartridge();
    for (size_t i = 0; i < cartridge.size(); i++) {
        EXPECT_EQ(_cartridge[i], cartridge[i]);
    }
}

TEST_F (MemoryTest, addNewEmptyCartridge)
{
    Memory mem;
    EXPECT_FALSE(mem.setCartridge(_emptyCartridge));
    std::vector<uint8_t> cartridge = mem.getCartridge();
    for (size_t i = 0; i < cartridge.size(); i++) {
        EXPECT_EQ(_cartridge[i], cartridge[i]);
    }
}

TEST_F (MemoryTest, addNewInvalidCartridge)
{
    Memory mem;
    std::vector<uint8_t> cartridge = { 0};
    EXPECT_FALSE(mem.setCartridge(cartridge));
}
