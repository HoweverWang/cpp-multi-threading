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
    std::future<int> fin = pin.get_future(); // 独占，不能被复制只能被移动，因此要引用传递
    std::shared_future<int> sfin = fin.share(); // 可以被复制，因此可以进行值传递

    // std::launch::async新线程执行; std::launch::deferred不创建新线程，仅仅延迟调用task(用到fout才调用)
    std::future<int> fout = std::async(std::launch::async, task, 2, sfin);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    pin.set_value(3);
    std::cout << fout.get() << std::endl; // f.get()会阻塞直到p得到结果。f.get()只能进行一次，否则会crash
    return 0;
}
