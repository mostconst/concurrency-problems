#include "sleepsort.h"

#include <string>
#include <thread>
#include <chrono>
#include <vector>

const auto sleepCoeff = std::chrono::milliseconds(20);

void sleepOnString(std::ostream& os, const std::string& s)
{
    std::this_thread::sleep_for(sleepCoeff * s.size());
    os << s << '\n';
}

void sleepsort(std::ostream& os, const std::vector<std::string>& strings)
{
    std::vector<std::thread> threads;
    for(const auto& s: strings)
    {
        threads.push_back(std::thread(sleepOnString, std::ref(os), s));
    }

    for (auto& t : threads)
        t.join();
}
