#include "sleepsort.h"
#include <gtest/gtest.h>

TEST(TestSleepsort, EmptyInput)
{
    std::istringstream is;
    std::ostringstream os;
    sleepsort(os, is);
    EXPECT_TRUE(os.str().empty());
}

TEST(TestSleepsort, OnelineInput)
{
    std::istringstream is("abc");
    std::ostringstream os;
    sleepsort(os, is);
    EXPECT_EQ(os.str(), "abc\n");
}

TEST(TestSleepsort, NormalInput)
{
    std::istringstream is("aaa\naa\na");
    std::ostringstream os;
    sleepsort(os, is);
    EXPECT_EQ(os.str(), "a\naa\naaa\n");
}