#include "queue.h"

MyMS::MyMS(const size_t capacity /*= 10*/)
    : capacity(capacity)
{

}

void MyMS::Drop()
{
    std::lock_guard guard(queue_mutex);
    queue = std::nullopt;
    queue_cv.notify_all();
}

bool MyMS::Put(const std::string& msg)
{
	std::unique_lock lock(queue_mutex);
    queue_cv.wait(lock, [this] {return !queue || queue->size() < capacity;});
	if (!queue)
	{
	    return false;
	}
	queue->push_back(msg);
    lock.unlock();

    queue_cv.notify_all();
    return true;
}

std::optional<std::string> MyMS::Get()
{
    std::unique_lock lock(queue_mutex);
    queue_cv.wait(lock, [this] {return !queue || !queue->empty();});
    std::optional<std::string> result = std::nullopt;
    if (queue)
    {
        result = queue->front();
        queue->pop_front();
    }
    lock.unlock();

    queue_cv.notify_all();
    return result;
}
