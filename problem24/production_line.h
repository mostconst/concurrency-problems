#pragma once
#include <semaphore>
#include <vector>
#include <functional>

class ProductionLogger
{
public:
    explicit ProductionLogger()
        : start(std::chrono::steady_clock::now())
    {}
    void Log(const std::string& message) const;
private:
    const std::chrono::steady_clock::time_point start;
};

class ProductionLine
{
public:
    ProductionLine(int time_to_make, std::string part_name,
        std::vector<std::reference_wrapper<ProductionLine>> supply_lines, const ProductionLogger& logger);

    void ProductionProcess();
    void Consume(bool another, const std::string& consumer);

public:
    const int timeToMake;
    const std::string partName;
    const std::string lineName;
    std::binary_semaphore semaphoreStart;
    std::binary_semaphore semaphoreDone;
    bool makeAnother = true;
    const std::vector<std::reference_wrapper<ProductionLine>> supplyLines;
    const ProductionLogger& logger;
};