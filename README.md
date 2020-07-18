# ThreadPool
基于生产者消费者模型的固定线程池
* 自定义线程数，添加任务
* 支持任务优先级（未添加）
* 支持日志文件保存（未添加）
  
###ThreadPools###c++线程池  
**样例:**  

```C++
#include <iostream>
#include <functional>
#include <string>
#include "ThreadPool.h"
using namespace std;


int main()
{
	int n = 10;
	auto func = [&n](int i) {
		n += 10;
		string s = "func: " + to_string(i) + " " + to_string(n);
		std::cout << s << endl;
	};

	Thread::thread_pool tp(2);
	tp.start(); 
	for (int i = 0; i < 10; ++i) {
		tp.add_one_task(func, i);
	}
	cout << "add task done" << endl;
	std::this_thread::sleep_for(2s);
	
	tp.stop();
	return 0;
}

```  
