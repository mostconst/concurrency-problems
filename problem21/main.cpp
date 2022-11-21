#include <iostream>
#include <string>

#include "list.h"

int main()
{
	MyTSlist list;
	std::string str;
	while (std::getline(std::cin, str))
	{
		if (!str.empty())
		{
			list.PushFront(str);
		}
		else
		{
			std::cout << list << std::endl;
		}
	}
	return 0;
}
