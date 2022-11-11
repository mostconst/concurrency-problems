#include <iostream>

#include "parallel-cp.h"

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage: parallel-cp SOURCE DEST" << std::endl;
		return 1;
	}

	bool res = parallelCopy(argv[1], argv[2]);
	return res ? 0 : 1;
}
