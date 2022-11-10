#include "parallel-cp.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

std::set<fs::path> listContent(const fs::path& dir, const fs::path& base)
{
    std::set<fs::path> res;
    for (auto const& dir_entry : fs::recursive_directory_iterator(dir))
    {
        res.insert(fs::relative(dir_entry.path(), base));
    }

    return res;
}

class CopyTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        fs::create_directories(testPath);
        fs::create_directories(destPath);
    }

    void TearDown() override
    {
        fs::remove_all(testAreaPath);
    }

    const fs::path testAreaPath = fs::temp_directory_path() / "test_area";
    const fs::path sourcePath = testAreaPath / "source";
    const fs::path testPath = sourcePath / "test_dir";
    const fs::path destPath = testAreaPath / "dest";

};

TEST_F(CopyTest, SetupCorrect)
{
    EXPECT_TRUE(fs::exists(testPath));
    EXPECT_TRUE(fs::exists(destPath));
}

TEST_F(CopyTest, EmptyDir)
{
    parallelCopy(testPath, destPath);
    EXPECT_EQ(listContent(sourcePath, sourcePath), listContent(destPath, destPath));
}

TEST_F(CopyTest, FlatStruct)
{
    std::ofstream(testPath / "a.txt");
    std::ofstream(testPath / "b.txt");
    parallelCopy(testPath, destPath);
    EXPECT_EQ(listContent(sourcePath, sourcePath), listContent(destPath, destPath));
}

TEST_F(CopyTest, NestedDirs)
{
    fs::create_directory(testPath / "dir1");
    fs::create_directory(testPath / "dir2");
    std::ofstream(testPath / "dir1" / "a.txt");
    std::ofstream(testPath / "dir2" / "b.txt");
    parallelCopy(testPath, destPath);
    EXPECT_EQ(listContent(sourcePath, sourcePath), listContent(destPath, destPath));
}

TEST_F(CopyTest, EmptyNested)
{
    fs::create_directory(testPath / "dir1");
    parallelCopy(testPath, destPath);
    EXPECT_EQ(listContent(sourcePath, sourcePath), listContent(destPath, destPath));
}