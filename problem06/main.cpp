#include <iostream>
#include <string>

#include "sleepsort.h"

int main()
{
    std::vector<std::string> input;
    for (std::string s; std::getline(std::cin, s);)
    {
        input.push_back(s);
    }
	sleepsort(std::cout, input);
	return 0;
}
