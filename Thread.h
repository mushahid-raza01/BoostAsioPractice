#include "Utility.h"

class Thread
{
public:
	Thread(DBL::THREAD_START_ROUTINE func_ptr, void* obj)
	{
		th_id = 0;
		DBL::thread_spawn(func_ptr,obj,th_id);
	}

	void join()
	{
		DBL::thread_join(th_id);
	}

	void detach()
	{
		DBL::thread_detach(th_id);
	}

	int get_id()
	{
		return (int)th_id;
	}
	
	
private:
	DBL::thread_id th_id;
	void(*func_ptr)(void* ptr);
	//void (*foo)(int);
    //foo = &my_int_func;
 
};