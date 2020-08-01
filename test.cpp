#include <iostream>
#include<functional>
#include "thread_pool.h"

using namespace std;



void add(int& a, int b) {
	a = a + b;
	b = a + b;
}

void func(int x, int& n) {
	n += 10;
    string s = "func" + to_string(x);
	cout << s << endl;
    std::this_thread::sleep_for(1s);
}



int main()
{
	Thread::thread_pool tp(5);
	tp.start();
	int n = 123;
	for (int i = 0; i < 20; ++i) {
		tp.add_one_task(func, i, std::ref(n));
	}
	cout << "add task done" << endl;
    
    tp.stop_until_empty();
//	tp.stop();
//	return 0;
}

