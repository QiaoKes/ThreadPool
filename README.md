# thread_pool
基于生产者消费者模型的固定线程池
* 自定义线程数，添加任务
* 支持任务优先级（未添加）
* 支持日志文件保存

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

# logger
基于模板模式的线程安全日志类
* 自定义日志等级
* 支持控制台日志输出与本地日志保存
* 支持自定义扩展日志输出方法
* 支持windows、linux

**功能:**  
```
1.日志等级
  Debug	指出细粒度信息事件对调试应用程序是非常有帮助的，主要用于开发过程中打印一些运行信息。
  Info	消息在粗粒度级别上突出强调应用程序的运行过程。用于生产环境中输出程序运行的一些重要信息，但是不能滥用，避免打印过多的日志。
  Warning 表明会出现潜在错误的情形，有些信息不是错误信息，但是也要给程序员的一些提示。
  Error 指出虽然发生错误事件，但仍然不影响系统的继续运行。打印错误和异常信息，如果不想输出太多的日志，可以使用这个级别。
  Fatal 指出每个严重的错误事件将会导致应用程序的退出。重大错误，这种级别你可以直接停止程序了。。   
2.ConsoleLogger(控制台输出用的日志类) 
  用于在控制台输出日志。  
3.FileLogger(写文档用的日志类) 
  用于在本地文件保存日志 
```  
  
###thread_pool###c++线程池  
###logger###c++线程安全日志类   
**样例:**  

```C++
#include <iostream>
#include <functional>
#include <cmath>
#include "thread_pool.h"
#include "logger.h"

using namespace std;
using namespace Thread::Logger;

int main()
{
    ConsoleLogger Debug;
    
    auto is_prime = [](int n)->bool {
        if(n <= 1) return false;
        int sqrt_n = sqrt(n);
        for(int i = 2; i <= sqrt_n; ++i) {
            if(n % i == 0) return false;
        }
        return true;
    };

    auto prime_count = [&is_prime, &Debug](int& ans, int l, int r) {
        int cnt = 0;
        for(int i = l; i <= r; ++i) {
            cnt += is_prime(i);
        }
        __sync_fetch_and_add(&ans, cnt);
        Debug(Level::Info) << "This Thread " << "id: " << std::this_thread::get_id() << " count is " << cnt; 
    };

    Thread::thread_pool tp(5);
    tp.start();
    
    int ans = 0;
    const int N = 100000; 
    for (int i = 0; i < 10; ++i) {
        tp.add_one_task(prime_count, std::ref(ans), i*N+1, (i+1)*N);
    }

    tp.stop_until_empty();
    cout << ans << endl;
    return 0;
}
```  

```
[2020-08-04 13:59:38][Info] This Thread id: 140035137517312 count is 8392
[2020-08-04 13:59:38][Info] This Thread id: 140035171088128 count is 9592
[2020-08-04 13:59:38][Info] This Thread id: 140035137517312 count is 7560
[2020-08-04 13:59:38][Info] This Thread id: 140035162695424 count is 8013
[2020-08-04 13:59:38][Info] This Thread id: 140035137517312 count is 7408
[2020-08-04 13:59:38][Info] This Thread id: 140035154302720 count is 7863
[2020-08-04 13:59:38][Info] This Thread id: 140035145910016 count is 7678
[2020-08-04 13:59:38][Info] This Thread id: 140035171088128 count is 7445
[2020-08-04 13:59:38][Info] This Thread id: 140035162695424 count is 7323
[2020-08-04 13:59:38][Info] This Thread id: 140035137517312 count is 7224
78498
```

**日志类详细演示:**
```C++
#include "thread_pool.h"
#include "logger.h"

using namespace std;
using namespace Thread::Logger;

int main() 
{	
	//控制台日志
	ConsoleLogger Debug;
	//默认日志等级为debug
	Debug() << "this is debug level " << "test finished";
	//指定日至等级
	Debug(Level::Info) << "this is Info level " << "test finished";

	//默认日志文件名为 当前时间.log
	FileLogger FL1;
	//自定义日志名 默认会去掉非法字符
	FileLogger FL2("TEST.log");

	//默认日志等级为debug
	FL1() << "this is debug level " << "test finished";
	//指定日至等级
	FL1(Level::Info) << "this is Info level " << "test finished";

	return 0;
}


```

```
[2020-08-04 14:08:46][Debug]    this is debug level test finished
[2020-08-04 14:08:46][Info]     this is Info level test finished
```
