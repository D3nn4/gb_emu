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
    MOCK_METHOD0(getData, IMemory::CartridgeData());
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

TEST_F (MemoryTest, getCartridge)
{
    Memory mem;

    EXPECT_TRUE(mem.setCartridge(_cartridge));

    IMemory::CartridgeData cartridge = mem.getCartridge();
    for (size_t i = 0; i < cartridge.size(); i++) {
        EXPECT_EQ(_cartridge[i], cartridge[i]);
    }
}

TEST_F (MemoryTest, getRom)
{
    Memory mem;
    EXPECT_TRUE(mem.setCartridge(_cartridge));

    IMemory::RomData rom = mem.getReadOnlyMemory();
    for (size_t i = 0; i < _bank0.size(); i++) {
        EXPECT_EQ(_bank0[i], rom[i]);
    }
    for (size_t i = _bank0.size(); i < rom.size(); i++) {
        EXPECT_EQ(0, rom[i]);
    }
}

TEST_F (MemoryTest, writeInMemory)
{
    Memory mem;

    EXPECT_FALSE(mem.writeInMemory(0xff, 0x1000));
    IMemory::RomData rom = mem.getReadOnlyMemory();
    EXPECT_EQ(0, rom[0x8001]);
    EXPECT_TRUE(mem.writeInMemory(0xff, 0x8001));
    rom = mem.getReadOnlyMemory();
    EXPECT_EQ(0xff, rom[0x8001]);
}

TEST_F (MemoryTest, setAndGet8BitRegisters)
{
    Memory mem;

    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::A));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::F));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::B));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::C));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::D));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::E));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::H));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::L));

    mem.set8BitRegister(IMemory::REG8BIT::A, 0xff);
    mem.set8BitRegister(IMemory::REG8BIT::F, 0xfe);
    mem.set8BitRegister(IMemory::REG8BIT::B, 0xfd);
    mem.set8BitRegister(IMemory::REG8BIT::C, 0xfc);
    mem.set8BitRegister(IMemory::REG8BIT::D, 0xfb);
    mem.set8BitRegister(IMemory::REG8BIT::E, 0xfa);
    mem.set8BitRegister(IMemory::REG8BIT::H, 0xf9);
    mem.set8BitRegister(IMemory::REG8BIT::L, 0xf8);

    EXPECT_EQ(0xff, mem.get8BitRegister(IMemory::REG8BIT::A));
    EXPECT_EQ(0xfe, mem.get8BitRegister(IMemory::REG8BIT::F));
    EXPECT_EQ(0xfd, mem.get8BitRegister(IMemory::REG8BIT::B));
    EXPECT_EQ(0xfc, mem.get8BitRegister(IMemory::REG8BIT::C));
    EXPECT_EQ(0xfb, mem.get8BitRegister(IMemory::REG8BIT::D));
    EXPECT_EQ(0xfa, mem.get8BitRegister(IMemory::REG8BIT::E));
    EXPECT_EQ(0xf9, mem.get8BitRegister(IMemory::REG8BIT::H));
    EXPECT_EQ(0xf8, mem.get8BitRegister(IMemory::REG8BIT::L));
}

TEST_F (MemoryTest, setAndGet16BitRegisters)
{
    Memory mem;

    EXPECT_EQ(0x0000, mem.get16BitRegister(IMemory::REG16BIT::AF));
    EXPECT_EQ(0x0000, mem.get16BitRegister(IMemory::REG16BIT::BC));
    EXPECT_EQ(0x0000, mem.get16BitRegister(IMemory::REG16BIT::DE));
    EXPECT_EQ(0x0000, mem.get16BitRegister(IMemory::REG16BIT::HL));
    EXPECT_EQ(0x0000, mem.get16BitRegister(IMemory::REG16BIT::PC));
    EXPECT_EQ(0x0000, mem.get16BitRegister(IMemory::REG16BIT::SP));

    mem.set16BitRegister(IMemory::REG16BIT::AF, 0xffff);
    mem.set16BitRegister(IMemory::REG16BIT::BC, 0xfffe);
    mem.set16BitRegister(IMemory::REG16BIT::DE, 0xfffd);
    mem.set16BitRegister(IMemory::REG16BIT::HL, 0xfffc);
    mem.set16BitRegister(IMemory::REG16BIT::PC, 0xfffb);
    mem.set16BitRegister(IMemory::REG16BIT::SP, 0xfffa);

    EXPECT_EQ(0xffff, mem.get16BitRegister(IMemory::REG16BIT::AF));
    EXPECT_EQ(0xfffe, mem.get16BitRegister(IMemory::REG16BIT::BC));
    EXPECT_EQ(0xfffd, mem.get16BitRegister(IMemory::REG16BIT::DE));
    EXPECT_EQ(0xfffc, mem.get16BitRegister(IMemory::REG16BIT::HL));
    EXPECT_EQ(0xfffb, mem.get16BitRegister(IMemory::REG16BIT::PC));
    EXPECT_EQ(0xfffa, mem.get16BitRegister(IMemory::REG16BIT::SP));
}

TEST_F(MemoryTest, setAndUnsetFlags)
{
    Memory mem;

    EXPECT_FALSE(mem.isSetFlag(IMemory::FLAG::Z));
    mem.setFlag(IMemory::FLAG::Z);
    EXPECT_TRUE(mem.isSetFlag(IMemory::FLAG::Z));
    mem.unsetFlag(IMemory::FLAG::Z);
    EXPECT_FALSE(mem.isSetFlag(IMemory::FLAG::Z));

    EXPECT_FALSE(mem.isSetFlag(IMemory::FLAG::N));
    mem.setFlag(IMemory::FLAG::N);
    EXPECT_TRUE(mem.isSetFlag(IMemory::FLAG::N));
    mem.unsetFlag(IMemory::FLAG::N);
    EXPECT_FALSE(mem.isSetFlag(IMemory::FLAG::N));


    EXPECT_FALSE(mem.isSetFlag(IMemory::FLAG::H));
    mem.setFlag(IMemory::FLAG::H);
    EXPECT_TRUE(mem.isSetFlag(IMemory::FLAG::H));
    mem.unsetFlag(IMemory::FLAG::H);
    EXPECT_FALSE(mem.isSetFlag(IMemory::FLAG::H));

    EXPECT_FALSE(mem.isSetFlag(IMemory::FLAG::C));
    mem.setFlag(IMemory::FLAG::C);
    EXPECT_TRUE(mem.isSetFlag(IMemory::FLAG::C));
    mem.unsetFlag(IMemory::FLAG::C);
    EXPECT_FALSE(mem.isSetFlag(IMemory::FLAG::C));
}

TEST_F(MemoryTest, readInMemory)
{
    Memory mem;
    mem.setCartridge(_cartridge);

    EXPECT_EQ(0x00, mem.readInMemory(0x0000));
    EXPECT_EQ(0x05, mem.readInMemory(0x0005));
    EXPECT_EQ(0xff, mem.readInMemory(0x00ff));
}

TEST_F(MemoryTest, setAndUnsetBitIn8BitRegister)
{
    Memory mem;

    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::A));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::F));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::B));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::C));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::D));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::E));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::H));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::L));

    mem.setBitInRegister(0, IMemory::REG8BIT::A);
    EXPECT_EQ(0x01, mem.get8BitRegister(IMemory::REG8BIT::A));
    mem.setBitInRegister(1, IMemory::REG8BIT::F);
    EXPECT_EQ(0x02, mem.get8BitRegister(IMemory::REG8BIT::F));
    mem.setBitInRegister(2, IMemory::REG8BIT::B);
    EXPECT_EQ(0x04, mem.get8BitRegister(IMemory::REG8BIT::B));
    mem.setBitInRegister(3, IMemory::REG8BIT::C);
    EXPECT_EQ(0x08, mem.get8BitRegister(IMemory::REG8BIT::C));
    mem.setBitInRegister(4, IMemory::REG8BIT::D);
    EXPECT_EQ(0x10, mem.get8BitRegister(IMemory::REG8BIT::D));
    mem.setBitInRegister(5, IMemory::REG8BIT::E);
    EXPECT_EQ(0x20, mem.get8BitRegister(IMemory::REG8BIT::E));
    mem.setBitInRegister(6, IMemory::REG8BIT::H);
    EXPECT_EQ(0x40, mem.get8BitRegister(IMemory::REG8BIT::H));
    mem.setBitInRegister((int)Memory::FLAG::Z, IMemory::REG8BIT::L);
    EXPECT_EQ(0x80, mem.get8BitRegister(IMemory::REG8BIT::L));
    EXPECT_THROW(mem.setBitInRegister(9, IMemory::REG8BIT::L), Memory::MemoryException);

    mem.unsetBitInRegister(0, IMemory::REG8BIT::A);
    mem.unsetBitInRegister(1, IMemory::REG8BIT::F);
    mem.unsetBitInRegister(2, IMemory::REG8BIT::B);
    mem.unsetBitInRegister(3, IMemory::REG8BIT::C);
    mem.unsetBitInRegister(4, IMemory::REG8BIT::D);
    mem.unsetBitInRegister(5, IMemory::REG8BIT::E);
    mem.unsetBitInRegister(6, IMemory::REG8BIT::H);
    mem.unsetBitInRegister(7, IMemory::REG8BIT::L);
    EXPECT_THROW(mem.unsetBitInRegister(9, IMemory::REG8BIT::L), Memory::MemoryException);

    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::A));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::F));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::B));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::C));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::D));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::E));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::H));
    EXPECT_EQ(0x00, mem.get8BitRegister(IMemory::REG8BIT::L));
}


// TEST_F (MemoryTest, isBitIn8BitRegisterSet)
// {
//     Memory mem;

//     EXPECT_FALSE(mem.isSet(0, IMemory::REG8BIT::A));
//     EXPECT_FALSE(mem.isSet(1, IMemory::REG8BIT::F));
//     EXPECT_FALSE(mem.isSet(2, IMemory::REG8BIT::B));
//     EXPECT_FALSE(mem.isSet(3, IMemory::REG8BIT::C));
//     EXPECT_FALSE(mem.isSet(4, IMemory::REG8BIT::D));
//     EXPECT_FALSE(mem.isSet(5, IMemory::REG8BIT::E));
//     EXPECT_FALSE(mem.isSet(6, IMemory::REG8BIT::H));
//     EXPECT_FALSE(mem.isSet(7, IMemory::REG8BIT::L));

//     mem.setBitInRegister(0, IMemory::REG8BIT::A);
//     mem.setBitInRegister(1, IMemory::REG8BIT::F);
//     mem.setBitInRegister(2, IMemory::REG8BIT::B);
//     mem.setBitInRegister(3, IMemory::REG8BIT::C);
//     mem.setBitInRegister(4, IMemory::REG8BIT::D);
//     mem.setBitInRegister(5, IMemory::REG8BIT::E);
//     mem.setBitInRegister(6, IMemory::REG8BIT::H);
//     mem.setBitInRegister(7, IMemory::REG8BIT::L);

//     EXPECT_THROW(mem.isSet(9, IMemory::REG8BIT::A), Memory::MemoryException);
//     EXPECT_TRUE(mem.isSet(0, IMemory::REG8BIT::A));
//     EXPECT_TRUE(mem.isSet(1, IMemory::REG8BIT::F));
//     EXPECT_TRUE(mem.isSet(2, IMemory::REG8BIT::B));
//     EXPECT_TRUE(mem.isSet(3, IMemory::REG8BIT::C));
//     EXPECT_TRUE(mem.isSet(4, IMemory::REG8BIT::D));
//     EXPECT_TRUE(mem.isSet(5, IMemory::REG8BIT::E));
//     EXPECT_TRUE(mem.isSet(6, IMemory::REG8BIT::H));
//     EXPECT_TRUE(mem.isSet(7, IMemory::REG8BIT::L));
// }

// TEST_F (MemoryTest, isBitIn16BitRegisterSet)
// {
//     Memory mem;

//     EXPECT_FALSE(mem.isSet(0, IMemory::REG16BIT::AF));
//     EXPECT_FALSE(mem.isSet(1, IMemory::REG16BIT::BC));
//     EXPECT_FALSE(mem.isSet(2, IMemory::REG16BIT::DE));
//     EXPECT_FALSE(mem.isSet(3, IMemory::REG16BIT::HL));
//     EXPECT_FALSE(mem.isSet(4, IMemory::REG16BIT::PC));
//     EXPECT_FALSE(mem.isSet(5, IMemory::REG16BIT::SP));

//     mem.setBitInRegister(0, IMemory::REG16BIT::AF);
//     mem.setBitInRegister(1, IMemory::REG16BIT::BC);
//     mem.setBitInRegister(2, IMemory::REG16BIT::DE);
//     mem.setBitInRegister(3, IMemory::REG16BIT::HL);
//     mem.setBitInRegister(4, IMemory::REG16BIT::PC);
//     mem.setBitInRegister(5, IMemory::REG16BIT::SP);

//     EXPECT_THROW(mem.unsetBitInRegister(17, IMemory::REG16BIT::HL), Memory::MemoryException);
//     EXPECT_TRUE(mem.isSet(0, IMemory::REG16BIT::AF));
//     EXPECT_TRUE(mem.isSet(1, IMemory::REG16BIT::BC));
//     EXPECT_TRUE(mem.isSet(2, IMemory::REG16BIT::DE));
//     EXPECT_TRUE(mem.isSet(3, IMemory::REG16BIT::HL));
//     EXPECT_TRUE(mem.isSet(4, IMemory::REG16BIT::PC));
//     EXPECT_TRUE(mem.isSet(5, IMemory::REG16BIT::SP));
// }
