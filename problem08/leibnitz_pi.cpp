#include <vector>
#include <future>

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

double leibnitzPiMT(int nTerms, int nTreads)
{
    if (nTreads > nTerms)
        nTreads = nTerms;
    std::vector<std::future<double>> futures;
    int portion = nTerms / nTreads;
    for(int start = 0; start < nTerms; start += portion)
    {
        futures.push_back(std::async([start, portion] {
            return leibnitzSum(start, portion);
        }));
    }

    double sum = 0.0;
    for(auto& f: futures)
        sum += f.get();
    
    return sum * 4;
}

double leibnitzPiST(int nTerms)
{
    return leibnitzSum(0, nTerms) * 4;
}