#include <iostream>
#include <iomanip>
#include <limits>

#include "leibnitz_pi.h"

const int nTerms = 1'000'000'000;
void PrintUsage()
{
	std::cerr << "Usage: enter number of threads(positive integer)" << std::endl;
}

int getPositiveInt(const std::string& str)
{
	try {
		size_t pos;
		int n = std::stoi(str, &pos);
		if (pos != str.size() || n <= 0)
			throw std::invalid_argument("");
		return n;
	} catch (...) {
		throw std::invalid_argument("");
	}
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		PrintUsage();
		return 1;
	}

	try {
		int nThreads = getPositiveInt(argv[1]);
		std::cout << std::setprecision(15) << leibnitzPiMT(nTerms, nThreads) << std::endl;
	} catch (std::invalid_argument&) {
		PrintUsage();
		return 1;
	}
	
	return 0;
}
