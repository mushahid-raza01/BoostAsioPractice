#include "Mutex.h"
#include "Thread.h"
#include <iostream>

long long num = 0;
DBL::Mutex locker;
		

THREAD_PROC FuncA(void* a)
{
	for(long long i=0; i<1000000; i++)
	{
		locker.lock();
		num++;
		locker.unlock();
	}
	return 0;
}

THREAD_PROC FuncB(void* a)
{
	for(long long i=0; i<1000000; i++)
	{
			locker.lock();
			num++;
			locker.unlock();
	}
	return 0;
}

int main ()
{
	int abc;
	bool* var2 = new bool(true);
	Thread t1(&FuncA,var2);
	Thread t2(&FuncB,var2);
	/*std::cin >> abc;
	if (abc)
	{
		*var2 = false;
		std::cout<<"Value is False Now...\n";
	}*/
	t1.join();	
	t2.join();	
	delete var2;
	std::cout<<"I am ending... num = " << num << std::endl;
	/*std::cin.clear();
	std::cin.ignore(INT_MAX,'\n');
	std::cin>>abc;*/
	
	return 0;
}
