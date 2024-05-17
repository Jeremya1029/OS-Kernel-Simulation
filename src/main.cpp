#include"SimOS.h"
#include<deque>

int main()
{
	SimOS sim{ 3, 1000, 10 };
	bool allTestsClean{ true };

	if (sim.GetCPU() != NO_PROCESS)
	{
		allTestsClean = false;
		std::cout << "Test on the line 9 fails!" << std::endl;
	}

	sim.NewProcess();
	if (sim.GetCPU() != 1)
	{
		allTestsClean = false;
		std::cout << "Test on the line 16 fails!" << std::endl;
	}

	sim.DiskReadRequest(0, "file1.txt");
	if (sim.GetCPU() != NO_PROCESS)
	{
		allTestsClean = false;
		std::cout << "Test on the line 23 fails!" << std::endl;
	}


	if (sim.GetCPU() != 1)
	{
		allTestsClean = false;
		std::cout << "Test on the line 51 fails!" << std::endl;
	}

	std::deque<int> readyQueue{ sim.GetReadyQueue() };
	if (readyQueue.size() != 0)
	{
		allTestsClean = false;
		std::cout << "Test on the line 58 fails!" << std::endl;
	}

	sim.SimFork();
	readyQueue = sim.GetReadyQueue();
	if (readyQueue[0] != 2)
	{
		allTestsClean = false;
		std::cout << "Test on the line 66 fails!" << std::endl;
	}

	sim.TimerInterrupt();
	readyQueue = sim.GetReadyQueue();
	if (sim.GetCPU() != 2 || readyQueue[0] != 1)
	{
		allTestsClean = false;
		std::cout << "Test on the line 74 fails!" << std::endl;
	}

	sim.SimExit();
	readyQueue = sim.GetReadyQueue();
	if (sim.GetCPU() != 1 || readyQueue.size() != 0)
	{
		allTestsClean = false;
		std::cout << "Test on the line 82 fails!" << std::endl;
	}


	sim.SimWait();
	if (sim.GetCPU() != 1)
	{
		allTestsClean = false;
		std::cout << "Test on the line 97 fails!" << std::endl;
	}

	if (allTestsClean)
		std::cout << "These preliminary tests are passed" << std::endl;

	return 0;
}