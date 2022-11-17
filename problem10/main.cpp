#include <iostream>
#include <mutex>
#include <vector>
#include <thread>

#include "philosopher.h"

void RunSimulation(int nPhilosophers, int duration, int thinkingTime, int eatingTime)
{
    std::vector<std::mutex> forks(nPhilosophers);
    std::vector<std::thread> philosophers;
    Monitor monitor(nPhilosophers);
    auto end = std::chrono::steady_clock::now() + std::chrono::milliseconds(duration);
    for (int i = 0; i < nPhilosophers; ++i)
    {
        int f1 = i;
        int f2 = (i + 1) % nPhilosophers;
        if (i == 0)
            std::swap(f1, f2);
        philosophers.emplace_back(Philosopher(i, thinkingTime, eatingTime), end, std::ref(forks[f1]), std::ref(forks[f2]), std::ref(monitor));
    }

    for (auto& ph : philosophers)
        ph.join();
}

int main()
{
    RunSimulation(5, 20000, 5000, 2000);

	return 0;
}
