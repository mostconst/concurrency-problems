#include <vector>
#include <thread>
#include <numeric>

#include "leibnitz_pi.h"

double leibnitzSum(int startTerm, int nTerms)
{
    double sum = 0.0;
    for(int i = startTerm; i != startTerm + nTerms; ++i)
    {
        double term = ((i % 2 == 0) ? 1.0 : -1.0) / (2 * i + 1);
        sum += term;
    }
    
    return sum;
}

double leibnitzPiMT(int nTerms, int nThreads)
{
    if (nThreads > nTerms)
        nThreads = nTerms;
    auto task = [](int start, int terms, double* out)
    {
        *out = leibnitzSum(start, terms); 
    };

    std::vector<std::thread> threads;
    std::vector<double> sums(nThreads, 0.0);
    int portion = nTerms / nThreads;
    for(int batch = 0; batch < nThreads; ++batch)
    {
        threads.push_back(std::thread(task, batch * portion, portion, &sums[batch]));
    }

    for(auto& t: threads)
        t.join();
    
    return std::accumulate(sums.begin(), sums.end(), 0.0) * 4;
}

double leibnitzPiST(int nTerms)
{
    return leibnitzSum(0, nTerms) * 4;
}