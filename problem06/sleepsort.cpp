#include "sleepsort.h"

#include <string>

void sleepsort(std::ostream& os, std::istream& is)
{
    for(std::string s; std::getline(is, s);)
    {
        os << s << '\n';
    }
}
