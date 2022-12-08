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

ProductionLine::ProductionLine(int time_to_make, std::string part_name,
    std::vector<std::reference_wrapper<ProductionLine>> supply_lines, const ProductionLogger& logger)
    :  timeToMake(time_to_make),
       partName(std::move(part_name)),
       lineName(partName + " maker"),
       semaphoreStart(1),
       semaphoreDone(0),
       supplyLines(std::move(supply_lines)),
       logger(logger)
{
}

void ProductionLine::ProductionProcess()
{
    logger.Log(lineName + " started");
    while (true)
    {
        semaphoreStart.acquire();
        for (auto& line : supplyLines)
        {
            line.get().Consume(makeAnother, lineName);
        }

        if (makeAnother)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(timeToMake));
            logger.Log(lineName + " made " + partName);
        }
        else
        {
            logger.Log(lineName + " stopped");
            semaphoreDone.release();
            break;
        }

        semaphoreDone.release();
    }
}

void ProductionLine::Consume(const bool another, const std::string& consumer)
{
    semaphoreDone.acquire();
    logger.Log(consumer + " consumed " + partName);
    makeAnother = another;
    semaphoreStart.release();
}
