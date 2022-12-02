#pragma once

#include <optional>
#include <deque>
#include <mutex>
#include <condition_variable>

class MyMS final
{
public:
    explicit MyMS(size_t capacity = 10);
    void Drop();
    bool Put(const std::string& msg);
    std::optional<std::string> Get();
private:
    using QueueType = std::deque<std::string>;
    std::optional<QueueType> queue = QueueType{};
    const size_t capacity;

    std::mutex queue_mutex;
    std::condition_variable queue_cv;
};
