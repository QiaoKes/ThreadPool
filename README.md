# ThreadPool
基于生产者消费者模型的固定线程池
* 自定义线程数，添加任务
* 支持任务优先级（未添加）
* 支持日志文件保存（未添加）

**功能:**  
```
1.thread_pool(std::size_t)   
  初始化线程池线程数。   
2.void add_one_task(Func&&, Args...)  
  添加一个任务，待执行函数和它的参数。  
3.void stop()  
  停止当前所有线程，不执行任务队列剩下的任务。
4.void stop_until_empty()  
  等待任务队列内剩余任务执行完毕后，停止所有线程。  
```  
  
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
