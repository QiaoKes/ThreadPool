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
#include <cmath>
#include "thread_pool.h"

using namespace std;


int main()
{
    auto is_prime = [](int n)->bool {
        if(n <= 1) return false;
        int sqrt_n = sqrt(n);
        for(int i = 2; i <= sqrt_n; ++i) {
            if(n % i == 0) return false;
        }
        return true;
    };

    auto prime_count = [&is_prime](int& ans, int l, int r) {
        int cnt = 0;
        for(int i = l; i <= r; ++i) {
            cnt += is_prime(i);
        }
        __sync_fetch_and_add(&ans, cnt);
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
