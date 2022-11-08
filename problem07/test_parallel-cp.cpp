#include "parallel-cp.h"
#include <gtest/gtest.h>
#include <filesystem>

namespace fs = std::filesystem;

class CopyTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        fs::create_directory(testPath);
    }

    void TearDown() override
    {
        fs::remove_all(testPath);
    }

    const fs::path testPath = fs::temp_directory_path() /= "mytest";
};

TEST_F(CopyTest, SetupCorrect)
{
    EXPECT_TRUE(fs::exists(testPath));
}