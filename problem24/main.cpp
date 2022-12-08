#include <iostream>
#include <iomanip>
#include <string>
#include <thread>

#include "production_line.h"

int main()
{
    ProductionLine lineForA(1000, "A", {});
    ProductionLine lineForB(2000, "B", {});
    ProductionLine lineForC(3000, "C", {});
    ProductionLine lineForModule(100, "Module", { &lineForA, &lineForB });
    ProductionLine lineForWidget(100, "Widget", {&lineForModule, &lineForC});

    std::vector<std::thread> lineThreads;
    lineThreads.emplace_back(&ProductionLine::ProductionProcess, std::ref(lineForA));
    lineThreads.emplace_back(&ProductionLine::ProductionProcess, std::ref(lineForB));
    lineThreads.emplace_back(&ProductionLine::ProductionProcess, std::ref(lineForC));
    lineThreads.emplace_back(&ProductionLine::ProductionProcess, std::ref(lineForModule));
    lineThreads.emplace_back(&ProductionLine::ProductionProcess, std::ref(lineForWidget));

    constexpr int nWidgets = 3;
    for(int i = 0; i != nWidgets; ++i)
    {
        lineForWidget.Consume(i != nWidgets - 1);
    }

    for(auto& thread: lineThreads)
    {
        thread.join();
    }

    return EXIT_SUCCESS;
}
