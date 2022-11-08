#include "sleepsort.h"
#include <algorithm>
#include <numeric>
#include <gtest/gtest.h>

struct TestData
{
    std::vector<std::string> input;
    std::string output;
};

TestData GenerateTestData(std::vector<int> lengths)
{
    std::vector<std::string> input;
    for (int l : lengths)
        input.emplace_back(l, 'a');

    std::string output;
    std::sort(lengths.begin(), lengths.end());
    for (int l : lengths)
    {
        output.insert(output.size(), l, 'a');
        output.push_back('\n');
    }

    return { input, output };
}

TEST(SleepsortCorrect, EmptyInput)
{
    std::ostringstream os;
    sleepsort(os, {});
    EXPECT_TRUE(os.str().empty());
}

TEST(SleepsortCorrect, OnelineInput)
{
    std::ostringstream os;
    std::vector<std::string> input = { "abc" };
    sleepsort(os, input);
    EXPECT_EQ(os.str(), "abc\n");
}

TEST(SleepsortCorrect, TestDataWorks)
{
    auto testData = GenerateTestData({ 3, 2, 1 });
    std::vector<std::string> exp_input = { "aaa", "aa", "a" };
    EXPECT_EQ(testData.input, exp_input);
    EXPECT_EQ(testData.output, "a\naa\naaa\n");
}
TEST(SleepsortCorrect, SimpleCase)
{
    auto testData = GenerateTestData({ 10, 1 });
    std::ostringstream os;
    sleepsort(os, testData.input);
    EXPECT_EQ(os.str(), testData.output);
}

TEST(SleepsortStable, WorstCase)
{
    int n = 100;
    std::vector<int> lengths(n - 1, 0);
    std::iota(lengths.begin(), lengths.end(), 2);
    lengths.push_back(1);
    // lengths: [2, ..., n-1, 1]
    EXPECT_EQ(lengths.size(), n);
    auto testData = GenerateTestData(lengths);
    for (int i = 0; i < 10; ++i)
    {
        std::ostringstream os;
        sleepsort(os, testData.input);
        EXPECT_EQ(os.str(), testData.output);
    }
}
