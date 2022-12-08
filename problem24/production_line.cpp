#include <vector>
#include <thread>
#include <iostream>
#include <syncstream>

#include "production_line.h"

void ProductionLine::ProductionProcess()
{
    std::osyncstream(std::cout) << "Maker of " << partName << " started" << std::endl;
    while (true)
    {
        semaphoreStart.acquire();
        for (auto& line : supplyLines)
        {
            line->Consume(makeAnother);
        }

        if (makeAnother)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(timeToMake));
            std::osyncstream(std::cout) << "Made " << partName << std::endl;
        }
        else
        {
            std::osyncstream(std::cout) << "Maker of " << partName << " stopped" << std::endl;
            semaphoreDone.release();
            break;
        }

        semaphoreDone.release();
    }
}

void ProductionLine::Consume(bool another)
{
    semaphoreDone.acquire();
    makeAnother = another;
    semaphoreStart.release();
}
