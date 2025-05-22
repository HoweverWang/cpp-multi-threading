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
#include <future>

int task(int a, std::shared_future<int> b) {
    int ret_a = a * a;
    int int_b = b.get();
    return ret_a + int_b * int_b;
}

int main()
{
    std::promise<int> pin;
    std::future<int> fin = pin.get_future();
    std::shared_future<int> sfin = fin.share();

    // std::packaged_task 本身并不创建线程，需要显式地在某个线程中调用（或通过 std::async 启动）
    std::packaged_task<int(int, std::shared_future<int>)> packed_task(task);
    std::future<int> fout = packed_task.get_future();
    std::thread t1(std::move(packed_task), 2, sfin);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    pin.set_value(3);
    std::cout << fout.get() << std::endl;
    t1.join();
    return 0;
}
