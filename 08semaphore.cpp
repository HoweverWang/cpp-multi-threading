#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <algorithm>
#include <functional> // for std::ref
#include <cmath>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <semaphore> // 内部关键状态（如计数器）是通过 std::atomic 实现的
// 但是，信号量控制的只是进入线程数量，并不会自动保护临界资源的访问。因此对于共享数据，仍需要手动加锁来避免数据竞争。

// LeastMaxValue = 6
std::counting_semaphore<6> csem(0);
// LeastMaxValue = 1
std::binary_semaphore bsem(0);

// producer
void task(int thread_index) {
    for (int i=0; i<30; i++) {
        // std::cout << thread_index << ": ready to recv signal: acquire\n";
        bsem.acquire(); // 如果信号量>1则信号量-1，否则阻塞等待
        std::cout << thread_index << ": acquired\n";
    }
}


int main() {
    std::thread t1(task, 1);
    std::thread t2(task, 2);
    for (int i=0; i<100; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "ready to signal: release\n";
        bsem.release(); // 信号量+1(已经达到最大值则保持不变)并通知等待的线程
        std::cout << "signalled\n";
    }
    if (t1.joinable()) t1.join();
    if (t2.joinable()) t2.join();
    return 0;
}
