#include "parallel-cp.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

std::set<fs::path> listContent(const fs::path& dir)
{
    std::set<fs::path> res;
    for (auto const& dir_entry : fs::recursive_directory_iterator(dir))
    {
        res.insert(fs::relative(dir_entry.path(), dir));
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
    EXPECT_EQ(listContent(sourcePath), listContent(destPath));
}

TEST_F(CopyTest, FlatStruct)
{
    std::ofstream(testPath / "a.txt");
    std::ofstream(testPath / "b.txt");
    bool res = parallelCopy(testPath, destPath);
    EXPECT_TRUE(res);
    EXPECT_EQ(listContent(sourcePath), listContent(destPath));
}

TEST_F(CopyTest, NestedDirs)
{
    fs::create_directory(testPath / "dir1");
    fs::create_directory(testPath / "dir2");
    std::ofstream(testPath / "dir1" / "a.txt");
    std::ofstream(testPath / "dir2" / "b.txt");
    parallelCopy(testPath, destPath);
    EXPECT_EQ(listContent(sourcePath), listContent(destPath));
}

TEST_F(CopyTest, EmptyNested)
{
    fs::create_directory(testPath / "dir1");
    parallelCopy(testPath, destPath);
    EXPECT_EQ(listContent(sourcePath), listContent(destPath));
}

TEST_F(CopyTest, MissingSource)
{
    bool res = parallelCopy(testPath / "missing_dir", destPath);
    EXPECT_FALSE(res);
    EXPECT_EQ(std::set<fs::path>{}, listContent(destPath));
    //EXPECT_THROW(parallelCopy(testPath, "/missing_dir"), fs::filesystem_error);
}

TEST_F(CopyTest, DifferentDestName)
{
    std::ofstream(testPath / "a.txt");
    bool res = parallelCopy(testPath, destPath / "another_name");
    EXPECT_TRUE(res);
    EXPECT_EQ(listContent(testPath), listContent(destPath / "another_name"));
    //EXPECT_THROW(parallelCopy(testPath, "/missing_dir"), fs::filesystem_error);
}

TEST_F(CopyTest, DifferentDestNameMissingParent)
{
    std::ofstream(testPath / "a.txt");
    auto dest = destPath / "missing_dir" / "another_name";
    bool res = parallelCopy(testPath, dest);
    EXPECT_FALSE(res);
    EXPECT_EQ(std::set<fs::path>{}, listContent(destPath));
    //EXPECT_THROW(parallelCopy(testPath, "/missing_dir"), fs::filesystem_error);
}

TEST_F(CopyTest, DestUnavailable)
{
    std::ofstream(testPath / "a.txt");
    fs::permissions(destPath, fs::perms::owner_read);
    bool res = parallelCopy(testPath, destPath);
    fs::permissions(destPath, fs::perms::all);
    EXPECT_FALSE(res);
    EXPECT_EQ(std::set<fs::path>{}, listContent(destPath));
    //EXPECT_EQ(listContent(sourcePath), listContent(destPath));
}

TEST_F(CopyTest, SourceWOReadPerm)
{
    std::ofstream(testPath / "a.txt");
    std::ofstream(testPath / "b.txt");
    fs::permissions(testPath, fs::perms::owner_read);
    bool res = parallelCopy(testPath, destPath);
    fs::permissions(testPath, fs::perms::all);
    EXPECT_FALSE(res);
    auto expContent = std::set<fs::path>{"test_dir"};
    EXPECT_EQ(expContent, listContent(destPath));
}

TEST_F(CopyTest, FileWOReadPerms)
{
    std::ofstream(testPath / "a.txt");
    std::ofstream(testPath / "b.txt");
    fs::permissions(testPath / "b.txt", fs::perms::owner_write);
    bool res = parallelCopy(testPath, destPath);
    EXPECT_FALSE(res);
    auto expContent = std::set<fs::path>{"test_dir", "test_dir/a.txt"};
    EXPECT_EQ(expContent, listContent(destPath));
    //fs::permissions(testPath / "b.txt", fs::perms::all);
}

TEST_F(CopyTest, DirWOReadPerms)
{
    fs::create_directory(testPath / "dir1");
    fs::create_directory(testPath / "dir2");
    std::ofstream(testPath / "dir1" / "a.txt");
    std::ofstream(testPath / "dir2" / "b.txt");
    fs::permissions(testPath / "dir2" / "b.txt", fs::perms::owner_write);
    bool res = parallelCopy(testPath, destPath);
    fs::permissions(testPath / "dir2" / "b.txt", fs::perms::all);
    EXPECT_FALSE(res);
    auto expContent = std::set<fs::path>{"test_dir", "test_dir/dir1", "test_dir/dir1/a.txt", "test_dir/dir2"};
    EXPECT_EQ(expContent, listContent(destPath));
}