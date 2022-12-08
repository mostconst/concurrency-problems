#pragma once
#include <semaphore>

class ProductionLine
{
public:
    ProductionLine(int time_to_make, std::string part_name,
                   std::vector<ProductionLine*> other_lines)
        : timeToMake(time_to_make),
          partName(std::move(part_name)),
          semaphoreStart(1),
          semaphoreDone(0),
          supplyLines(std::move(other_lines))
    {
    }

    void ProductionProcess();
    void Consume(bool another);

public:
    const int timeToMake;
    const std::string partName;
    std::binary_semaphore semaphoreStart;
    std::binary_semaphore semaphoreDone;
    bool makeAnother = true;
    const std::vector<ProductionLine*> supplyLines;
};

void productionLineDriver(ProductionLine& l);