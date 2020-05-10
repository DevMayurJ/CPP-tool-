// ThreadSafeQueue.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SafeQueue.h"
#include <thread>         // std::thread
#include <mutex>


void DelQ_thread(SafeQueue<int> *pobjSafeQueue)
{
	for (int i = 0; i < 2000; ++i)
		pobjSafeQueue->enqueue(i);
}

void AddQ_thread(SafeQueue<int> *pobjSafeQueue)
{
	while (1)
	{
		int iRet = pobjSafeQueue->dequeue();
		std::cout << iRet << " ";
	}
}

int main()
{
	std::thread threads[2];

	SafeQueue<int> objSafeQueue;
	threads[0] = std::thread(DelQ_thread, &objSafeQueue);
	threads[1] = std::thread(AddQ_thread, &objSafeQueue);

	threads[0].join();
	threads[1].join();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
