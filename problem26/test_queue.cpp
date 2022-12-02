#include "queue.h"
#include <gtest/gtest.h>

#include <future>
TEST(TestQueue, WorksAsQueue)
{
    MyMS q;
    ASSERT_TRUE(q.Put("1"));
    ASSERT_TRUE(q.Put("2"));
    ASSERT_TRUE(q.Put("3"));
    const auto item1 = q.Get();
    ASSERT_TRUE(item1);
    const auto item2 = q.Get();
    ASSERT_TRUE(item2);
    const auto item3 = q.Get();
    ASSERT_TRUE(item3);
    if (item1 && item2 && item3)
    {
        ASSERT_EQ(*item1, "1");
        ASSERT_EQ(*item2, "2");
        ASSERT_EQ(*item3, "3");
    }
}

TEST(TestQueue, DropEmpty)
{
    MyMS q;
    q.Drop();
    ASSERT_FALSE(q.Put("1"));
    ASSERT_FALSE(q.Get());
}

TEST(TestQueue, DropNonempty)
{
    MyMS q;
    ASSERT_TRUE(q.Put("1"));
    ASSERT_TRUE(q.Put("2"));
    ASSERT_TRUE(q.Put("3"));
    q.Drop();
    ASSERT_FALSE(q.Put("4"));
    ASSERT_FALSE(q.Get());
}

std::vector<std::string> buildStringsRange(const int begin, const int end)
{
    std::vector<std::string> res;
    for (int i = begin; i != end; ++i)
    {
        res.push_back(std::to_string(i));
    }

    return res;
}

std::vector<std::string> readUntilDropped(MyMS& queue)
{
    std::vector<std::string> res;
    for (;;)
    {
        auto msg = queue.Get();
        if (!msg)
        {
            break;
        }
        res.push_back(*msg);
    }

    return res;
}

int writeUntilDropped(MyMS& queue)
{
    int written = 0;
    while (true)
    {
        if (!queue.Put(std::to_string(written)))
        {
            break;
        }
        ++written;
    }

    return written;
}

using namespace std::chrono_literals;

int writeConseqN(MyMS& queue, const int total, std::atomic<int>& left, const std::chrono::milliseconds pause)
{
    int written = 0;
    int curr;
    while((curr = left--) > 0)
    {
        std::this_thread::sleep_for(pause);
        if (queue.Put(std::to_string(total - curr)))
        {
            ++written;
        }
    }

    return written;
}

std::vector<std::string> readConseqN(MyMS& queue, std::atomic<int>& n, const std::chrono::milliseconds pause)
{
    std::vector<std::string> res;
    while (n-- > 0)
    {
        std::this_thread::sleep_for(pause);
        if (auto item = queue.Get())
        {
            res.push_back(*item);
        }
    }

    return res;
}

TEST(TestQueue, ReadWriteN)
{
    MyMS q;
    constexpr int nMessages = 100000;
    constexpr int nWriters = 2;
    constexpr int nReaders = 2;

    std::atomic<int> toWrite = nMessages;
    std::vector<std::future<int>> writers;
    for(int i = 0; i != nWriters; ++i)
    {
        writers.push_back(std::async(std::launch::async, writeConseqN, std::ref(q), nMessages, std::ref(toWrite), 0ms));
    }

    std::atomic<int> toRead = nMessages;
    std::vector<std::future<std::vector<std::string>>> readers;
    for (int i = 0; i != nReaders; ++i)
    {
        readers.push_back(std::async(std::launch::async, readConseqN, std::ref(q), std::ref(toRead), 0ms));
    }

    for(auto& w: writers)
    {
        w.get();
    }

    std::vector<std::string> readMessages;
    for (auto& r : readers)
    {
        std::vector<std::string> messagesFromReader = r.get();
        readMessages.insert(readMessages.end(), messagesFromReader.begin(), messagesFromReader.end());
    }
    std::sort(readMessages.begin(), readMessages.end(), 
        [](const auto& a, const auto& b) {return std::stoi(a) < std::stoi(b); });

    ASSERT_EQ(readMessages, buildStringsRange(0, nMessages));
}

TEST(TestQueue, WriteBound)
{
    MyMS q;
    constexpr int nMessages = 10;

    std::atomic<int> toWrite = nMessages;
    auto writer = std::async(std::launch::async, writeConseqN, std::ref(q), nMessages, std::ref(toWrite), 100ms);

    std::atomic<int> toRead = nMessages;
    auto reader = std::async(std::launch::async, readConseqN, std::ref(q), std::ref(toRead), 0ms);

    writer.get();
    ASSERT_EQ(reader.get(), buildStringsRange(0, nMessages));
}

TEST(TestQueue, ReadBound)
{
    constexpr int capacity = 10;
    MyMS q(capacity);
    constexpr int nMessages = 10;

    auto writer = std::async(std::launch::async, writeUntilDropped, std::ref(q));

    std::atomic<int> toRead = nMessages;
    auto reader = std::async(std::launch::async, readConseqN, std::ref(q), std::ref(toRead), 100ms);

    ASSERT_EQ(reader.get(), buildStringsRange(0, nMessages));
    q.Drop();
    const int written = writer.get();
    ASSERT_GE(written, nMessages);
    ASSERT_LE(written, nMessages + capacity);
}

TEST(TestQueue, DropReaders)
{
    MyMS q;
    auto writeTask = [&q]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        q.Drop();
    };

    auto writer = std::async(std::launch::async, writeTask);
    std::vector<std::future<std::vector<std::string>>> readers;
    constexpr int nReaders = 3;
    for (int i = 0; i != nReaders; ++i)
    {
        readers.push_back(std::async(std::launch::async, readUntilDropped, std::ref(q)));
    }

    writer.get();
    for (auto& r: readers)
    {
        r.get();
    }
}

TEST(TestQueue, DropWriters)
{
    constexpr int capacity = 1000;
    MyMS q(capacity);
    auto readTask = [&q]()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        q.Drop();
    };

    auto reader = std::async(std::launch::async, readTask);
    std::vector<std::future<int>> writers;
    constexpr int nWriters = 3;
    for (int i = 0; i != nWriters; ++i)
    {
        writers.push_back(std::async(std::launch::async, writeUntilDropped, std::ref(q)));
    }

    reader.get();
    int totalWritten = 0;
    for (auto& r : writers)
    {
        totalWritten += r.get();
    }
    ASSERT_EQ(totalWritten, capacity);
}
