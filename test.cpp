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

