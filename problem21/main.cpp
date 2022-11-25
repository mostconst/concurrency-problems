#include <iostream>
#include <string>
#include <chrono>

#include "list.h"
#include "synchronized.h"

void SortingTask(MyTSlist& list, Synchronized<bool>& stopCond)
{
    while (stopCond.GetAccess().ref_to_value)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "Started sorting" << std::endl;
        std::cout << list << std::endl;
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            int exchanges = list.SortStep();
            if (!exchanges)
            {
                break;
            }
            std::cout << list << std::endl;
        }
        std::cout << "Finished sorting" << std::endl;
    }
}

int main()
{
	MyTSlist list;
	std::string str;
    Synchronized<bool> active;
    active.GetAccess().ref_to_value = true;
    std::thread sortThread(SortingTask, std::ref(list), std::ref(active));
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
    active.GetAccess().ref_to_value = false;
    sortThread.join();

	return 0;
}
