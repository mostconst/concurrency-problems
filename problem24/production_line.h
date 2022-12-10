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

class PartStorage final
{
public:
    explicit PartStorage(std::string part_name)
        : partName(std::move(part_name))
    {
    }

    [[nodiscard]] std::string GetPartName() const
    {
        return partName;
    }

    void Put()
    {
        parts.release();
    }

    void Consume()
    {
        parts.acquire();
    }
private:
    const std::string partName;

private:
    std::counting_semaphore<> parts{0};
};

class ProductionStage final
{
public:
    ProductionStage(int time_to_make, std::string stage_name, PartStorage& finished_parts,
        std::vector<std::reference_wrapper<PartStorage>> supplies, const ProductionLogger& logger);

    void Make() const;
public:
    [[nodiscard]] const std::string& GetStageName() const
    {
        return stageName;
    }

private:
    const int timeToMake;
    const std::string stageName;
    PartStorage& finishedParts;
    const std::vector<std::reference_wrapper<PartStorage>> supplies;
    const ProductionLogger& logger;
};

void workerRoutine(ProductionStage& stage, const ProductionLogger& logger, int plan);