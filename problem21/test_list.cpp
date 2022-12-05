#include "list.h"
#include "synchronized.h"
#include <gtest/gtest.h>

#include <future>

using namespace std::chrono_literals;

std::vector<std::string> toVectorGeneral(const MyTSlist& l, int max, std::chrono::milliseconds pause)
{
    std::vector<std::string> res;
    auto listIter = l.GetIterator();
    while (listIter.hasNext())
    {
        res.push_back(listIter.Next());
        if ((int)res.size() == max)
        {
            break;
        }
        std::this_thread::sleep_for(pause);
    }

    return res;
}

std::vector<std::string> toVector(const MyTSlist& l, int max = -1)
{
    return toVectorGeneral(l, max, 0ms);
}

std::vector<std::string> toVectorWithPauses(const MyTSlist& l, std::chrono::milliseconds pause)
{
    return toVectorGeneral(l, -1, pause);
}

TEST(TestList, PrintEmpty)
{
    MyTSlist l;
    std::ostringstream stream;
    stream << l;
    EXPECT_TRUE(stream.str().empty());
}

TEST(TestList, PrintOneEl)
{
    MyTSlist l;
    l.PushFront("a");
    std::ostringstream stream;
    stream << l;
    EXPECT_EQ(stream.str(), "a");
}

TEST(TestList, PrintOverOneEl)
{
    MyTSlist l;
    l.PushFront("b");
    l.PushFront("a");
    std::ostringstream stream;
    stream << l;
    EXPECT_EQ(stream.str(), "a -> b");
}

TEST(TestList, Insert)
{
    MyTSlist l;
    l.PushFront("b");
    {
        EXPECT_EQ(toVector(l), std::vector<std::string>{"b"});
    }
    l.PushFront("a");
    {
        const std::vector<std::string> expected{ "a", "b" };
        EXPECT_EQ(toVector(l), expected);
    }
}

TEST(TestList, SortEmpty)
{
    MyTSlist l;
    l.Sort();
    EXPECT_EQ(toVector(l), std::vector<std::string>{});
}

TEST(TestList, SortOneEl)
{
    MyTSlist l;
    l.PushFront("a");
    l.Sort();
    EXPECT_EQ(toVector(l), std::vector<std::string>{"a"});
}

TEST(TestList, SortTwoEl)
{
    MyTSlist l;
    l.PushFront("a");
    l.PushFront("b");
    l.Sort();
    const std::vector<std::string> expected{ "a", "b" };
    EXPECT_EQ(toVector(l), expected);
}

TEST(TestList, SortGeneral)
{
    MyTSlist l;
    l.PushFront("a");
    l.PushFront("b");
    l.PushFront("c");
    l.PushFront("d");
    l.Sort();
    const std::vector<std::string> expected{ "a", "b", "c", "d"};
    EXPECT_EQ(toVector(l), expected);
}
TEST(TestList, SerialUpdate)
{
    MyTSlist l;

    const size_t add_count = 100;
    auto updater = [&l, add_count] {
        for (size_t i = 0; i < add_count; ++i) {
            l.PushFront("a");
        }
    };

    updater();
    updater();

    ASSERT_EQ(add_count * 2, toVector(l).size());
}

TEST(TestList, ConcurrentUpdate)
{
    MyTSlist l;

    const size_t add_count = 50000;
    auto updater = [&l, add_count]
    {
        for (size_t i = 0; i < add_count; ++i)
        {
            l.PushFront("a");
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i)
    {
        threads.emplace_back(updater);
    }

    for (auto& t : threads)
    {
        t.join();
    }

    ASSERT_EQ(add_count * 4, toVector(l).size());
}

TEST(TestList, ConcurrentSort)
{
    const std::vector<std::string> expected = []()
    {
        std::vector<std::string> res;
        for (char c1 = 'a'; c1 < 'f'; ++c1)
        {
            for (char c2 = 'a'; c2 < 'z'; ++c2)
            {
                res.push_back(std::string(1, c1) + std::string(1, c2));
            }
        }
        return res;
    }();

    MyTSlist l;
    for (const auto& s : expected)
    {
        l.PushFront(s);
    }

    auto sortTask = [&l]()
    {
        l.Sort();
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i)
    {
        threads.emplace_back(sortTask);
    }

    for (auto& t: threads)
    {
        t.join();
    }

    auto actual = toVector(l);
    ASSERT_EQ(expected, actual);
}

std::vector<std::string> GetConseq(const int start, const int n)
{
    std::vector<std::string> res;
    for (int i = 0; i < n; ++i)
    {
        res.push_back(std::to_string(start - i));
    }

    return res;
}
TEST(TestList, ReadingWhileInserting)
{
    MyTSlist l;
    Synchronized<bool> stopSignal;
    stopSignal.GetAccess().ref_to_value = false;
    const size_t add_count = 50000;
    auto writer = [&l, add_count, &stopSignal]
    {
        for (size_t i = 0; i < add_count; ++i)
        {
            l.PushFront(std::to_string(i));
        }
        stopSignal.GetAccess().ref_to_value = true;
    };

    int nRead = 3;
    auto readFirstFew = [&l, &stopSignal, &nRead]
    {
        std::vector<std::vector<std::string>> readResults;
        while (true)
        {
            bool stop = stopSignal.GetAccess().ref_to_value;
            readResults.push_back(toVector(l, nRead));
            if (stop)
            {
                break;
            }
        }
        return readResults;
    };

    auto w = std::async(std::launch::async, writer);
    auto r1 = std::async(std::launch::async, readFirstFew);
    auto r2 = std::async(std::launch::async, readFirstFew);
    auto r3 = std::async(std::launch::async, readFirstFew);

    w.get();
    for (auto f : { &r1, &r2, &r3 })
    {
        auto readRes = f->get();
        ASSERT_TRUE(!readRes.empty());
        ASSERT_TRUE(!readRes.back().empty());
        ASSERT_EQ(readRes.back(), GetConseq(add_count - 1, nRead));
        for (const auto& head : readRes)
        {
            if (!head.empty())
            {
                ASSERT_EQ(head, GetConseq(std::stoi(head.front()), head.size()));
            }
        }
    }
}

TEST(TestList, ClearEmpty)
{
    MyTSlist l;
    l.Clear();
    ASSERT_EQ(toVector(l), std::vector<std::string>{});
}

TEST(TestList, ClearNonempty)
{
    MyTSlist l;
    l.PushFront("1");
    l.PushFront("2");
    l.Clear();
    ASSERT_EQ(toVector(l), std::vector<std::string>{});
}

TEST(TestList, ConcurrentIterateAndClear)
{
    MyTSlist l;
    int nElem = 100;
    for(int i = 0; i < nElem; ++i)
    {
        l.PushFront(std::to_string(i));
    }

    auto reader = std::async(std::launch::async, toVectorWithPauses, std::ref(l), 10ms);
    std::this_thread::sleep_for(100ms);
    auto cleaner = std::async(std::launch::async, [&l] {l.Clear(); });
    auto listContent = reader.get();
    cleaner.get();
    ASSERT_EQ(toVector(l), std::vector<std::string>{});
    ASSERT_EQ(listContent, GetConseq(nElem - 1, nElem));
}