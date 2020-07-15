#include <iostream>
#include<functional>
#include "ThreadPool.h"
using namespace std;




void func(int i, int& n) {
	n += 10;
	cout << "func"   << " " << i  << " " << n <<  endl;
}



int main()
{
    Thread::thread_pool tp(3);
	int n = 123;
    tp.start();
	for (int i = 0; i < 10; ++i) {
		tp.add_one_task(func, i, std::ref(n));
	}
	cout << "add task done" << endl;
	std::this_thread::sleep_for(2s);
	

	//tp.stop();
	
	return 0;
}

