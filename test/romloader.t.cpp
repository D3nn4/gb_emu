#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "romloader.hpp"
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

class RomLoaderTest : public ::testing::Test
{
public:
    MockFileIo fileIO;
    std::string fileName = "../roms/Pokemon_Bleue.gb";
    int fd = 42;
private:
};

TEST_F (RomLoaderTest, loadValidFile)
{
    EXPECT_CALL(fileIO, openFile(fileName))
        .WillOnce(Return(fd));

    std::vector<uint8_t> firstBuffer(512,7);
    std::vector<uint8_t> secondBuffer(35,2);

    EXPECT_CALL(fileIO, readFile(_, fd))
        .WillOnce(DoAll(SetArrayArgument<0>(firstBuffer.begin(), firstBuffer.end()),
                        Return(firstBuffer.size())))
        .WillOnce(DoAll(SetArrayArgument<0>(secondBuffer.begin(), secondBuffer.end()),
                        Return(secondBuffer.size())))
        .WillOnce(Return(0));

    EXPECT_CALL(fileIO, closeFile(fd));

    RomLoader romLoader(fileIO);
    bool isLoaded = romLoader.load(fileName);

    std::vector<uint8_t> data = romLoader.getData();
    for (int i = 0; i < 512; i++) {
        EXPECT_EQ(data[i], 7);
    }
    for (int i = 0; i < 35; i++) {
        EXPECT_EQ(data[i + 512], 2);
    }
    EXPECT_TRUE(isLoaded);
}


TEST_F (RomLoaderTest, loadUnvalidFile)
{
    EXPECT_CALL(fileIO, openFile(fileName))
        .WillOnce(Return(-1));

    RomLoader romLoader(fileIO);
    bool isLoaded = romLoader.load(fileName);

    EXPECT_FALSE(isLoaded);
}

TEST_F (RomLoaderTest, loadEmptyFile)
{
    EXPECT_CALL(fileIO, openFile(fileName))
        .WillOnce(Return(fd));

    EXPECT_CALL(fileIO, readFile(_, fd))
        .WillOnce(Return(0));

    EXPECT_CALL(fileIO, closeFile(fd));

    RomLoader romLoader(fileIO);
    bool isLoaded = romLoader.load(fileName);

    EXPECT_TRUE(romLoader.getData().empty());
    EXPECT_TRUE(isLoaded);
}
