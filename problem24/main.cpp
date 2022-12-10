#include <iostream>
#include <iomanip>
#include <string>
#include <thread>

#include "production_line.h"

int main()
{
    ProductionLogger logger;

    PartStorage finishedAs("A");
    PartStorage finishedBs("B");
    PartStorage finishedCs("C");
    PartStorage finishedModules("Module");
    PartStorage storageForWidget("Widget");

    ProductionStage stageForA(1000, "A stage", finishedAs, {}, logger);
    ProductionStage stageForB(2000, "B stage", finishedBs, {}, logger);
    ProductionStage stageForC(3000, "C stage", finishedCs, {}, logger);
    ProductionStage stageForModule(100, "Module stage", finishedModules, { finishedAs, finishedBs }, logger);
    ProductionStage stageForWidget(100, "Widget stage", storageForWidget, {finishedModules, finishedCs}, logger);

    constexpr int productionPlan = 3;
    std::vector<std::thread> workers;
    workers.emplace_back(workerRoutine, std::ref(stageForA), std::ref(logger), productionPlan);
    workers.emplace_back(workerRoutine, std::ref(stageForB), std::ref(logger), productionPlan);
    workers.emplace_back(workerRoutine, std::ref(stageForC), std::ref(logger), productionPlan);
    workers.emplace_back(workerRoutine, std::ref(stageForModule), std::ref(logger), productionPlan);
    workers.emplace_back(workerRoutine, std::ref(stageForWidget), std::ref(logger), productionPlan);

    for(auto& thread: workers)
    {
        thread.join();
    }

    return EXIT_SUCCESS;
}
