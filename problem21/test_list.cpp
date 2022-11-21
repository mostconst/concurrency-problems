#include "list.h"
#include <gtest/gtest.h>

TEST(TestList, PrintEmpty)
{
    MyTSlist l;
    std::ostringstream stream;
    stream << l;
    EXPECT_TRUE(stream.str().empty());
}

TEST(TestList, Insert)
{
    MyTSlist l;
    l.PushFront("b");
    {
        std::ostringstream stream;
        stream << l;
        EXPECT_EQ(stream.str(), "b ");
    }
    l.PushFront("a");
    {
        std::ostringstream stream;
        stream << l;
        EXPECT_EQ(stream.str(), "a b ");
    }
}

TEST(TestList, SortEmpty)
{
    MyTSlist l;
    l.Sort();
    {
        std::ostringstream stream;
        stream << l;
        EXPECT_EQ(stream.str(), "");
    }
}

TEST(TestList, SortOneEl)
{
    MyTSlist l;
    l.PushFront("a");
    l.Sort();
    {
        std::ostringstream stream;
        stream << l;
        EXPECT_EQ(stream.str(), "a ");
    }
}

TEST(TestList, SortTwoEl)
{
    MyTSlist l;
    l.PushFront("a");
    l.PushFront("b");
    l.Sort();
    {
        std::ostringstream stream;
        stream << l;
        EXPECT_EQ(stream.str(), "a b ");
    }
}

TEST(TestList, SortGeneral)
{
    MyTSlist l;
    l.PushFront("a");
    l.PushFront("b");
    l.PushFront("c");
    l.PushFront("d");
    l.Sort();
    {
        std::ostringstream stream;
        stream << l;
        EXPECT_EQ(stream.str(), "a b c d ");
    }
}