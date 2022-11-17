#pragma once

#include <mutex>
#include <map>
#include "synchronized.h"

using Timepoint = std::chrono::time_point<std::chrono::steady_clock>;

class Monitor;

class Philosopher
{
public:
    enum class State
    {
        Idle,
        Thinking,
        WaitingFor1stFork,
        WaitingFor2ndFork,
        Eating
    };

    Philosopher(int _id, int _thinkDuration, int _eatDuration):
        id(_id),
        thinkDuration(_thinkDuration),
        eatDuration(_eatDuration)
    {}

    void operator()(Timepoint end, std::mutex& firstFork, std::mutex& secondFork, Monitor& monitor);

private:
    void Think(Monitor& monitor);
    void Eat(std::mutex& firstFork, std::mutex& secondFork, Monitor& monitor);

private:
    int id;
    const int thinkDuration;
    const int eatDuration;
};

class Monitor
{
public:
    Monitor(int nPhilosophers)
        : states(nPhilosophers, Philosopher::State::Idle)
    {}
    void StateChange(int id, Philosopher::State newState);

private:
    std::vector<Philosopher::State> states;
    std::mutex mut;

    static const std::map<Philosopher::State, std::string> transitionMessages, stateStrings;
};