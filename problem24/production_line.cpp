#include <vector>
#include <thread>
#include <iostream>
#include <syncstream>

#include "production_line.h"

void ProductionLogger::Log(const std::string& message) const
{
    const auto finish = std::chrono::steady_clock::now();
    const auto dur = finish - start;
    std::osyncstream(std::cout) << std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() << " ms: " << message << std::endl;
}

ProductionStage::ProductionStage(int time_to_make, std::string stage_name, PartStorage& finished_parts,
    std::vector<std::reference_wrapper<PartStorage>> supplies, const ProductionLogger& logger)
    : timeToMake(time_to_make),
      stageName(std::move(stage_name)),
      finishedParts(finished_parts),
      supplies(std::move(supplies)),
      logger(logger)
{
}

void ProductionStage::Make() const
{
    for (auto& supply : supplies)
    {
        supply.get().Consume();
        logger.Log(stageName + " consumed " + supply.get().GetPartName());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(timeToMake));
    finishedParts.Put();
    logger.Log(stageName + " made " + finishedParts.GetPartName());
}

void workerRoutine(ProductionStage& stage, const ProductionLogger& logger, const int plan)
{
    logger.Log("worker started on " + stage.GetStageName());
    for(int i = 0; i != plan; ++i)
    {
        stage.Make();
    }
    logger.Log("worker stopped on " + stage.GetStageName());
}
