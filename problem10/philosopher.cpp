#include "philosopher.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <iomanip>

void Philosopher::operator()(Timepoint end, std::mutex& firstFork, std::mutex& secondFork, Monitor& monitor)
{
    while (std::chrono::steady_clock::now() < end)
    {
        Think(monitor);
        Eat(firstFork, secondFork, monitor);
    }
    monitor.StateChange(id, State::Idle);
}

void Philosopher::Think(Monitor& monitor)
{
    monitor.StateChange(id, State::Thinking);
    std::this_thread::sleep_for(std::chrono::milliseconds(thinkDuration));
}

void Philosopher::Eat(std::mutex& firstFork, std::mutex& secondFork, Monitor& monitor)
{
    monitor.StateChange(id, State::WaitingFor1stFork);
    std::lock_guard<std::mutex> firstGuard(firstFork);
    monitor.StateChange(id, State::WaitingFor2ndFork);
    std::lock_guard<std::mutex> secondGuard(secondFork);
    monitor.StateChange(id, State::Eating);
    std::this_thread::sleep_for(std::chrono::milliseconds(eatDuration));
}

const std::map<Philosopher::State, std::string> Monitor::transitionMessages = {
    {Philosopher::State::Idle, "left the table"},
    {Philosopher::State::Thinking, "started thinking"},
    {Philosopher::State::WaitingFor1stFork, "waiting for first fork"},
    {Philosopher::State::WaitingFor2ndFork, "waiting for second fork"},
    {Philosopher::State::Eating, "started eating"},
};

const std::map<Philosopher::State, std::string> Monitor::stateStrings = {
    {Philosopher::State::Idle, "|"},
    {Philosopher::State::Thinking, "?"},
    {Philosopher::State::WaitingFor1stFork, ".."},
    {Philosopher::State::WaitingFor2ndFork, "./"},
    {Philosopher::State::Eating, "//"},
};

void Monitor::StateChange(int id, Philosopher::State newState)
{
    std::lock_guard<std::mutex> guard(mut);
    states[id] = newState;

    std::cout << "Philosopher " << id << " " << transitionMessages.at(newState) << std::endl;
    for (const auto& s : states)
        std::cout << std::setw(3) << stateStrings.at(s);
    std::cout << std::endl;
}
