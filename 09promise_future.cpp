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

// void task(int a, std::future<int>& b, std::promise<int>& p) {
//     int ret_a = a * a;
//     int int_b = b.get();
//     p.set_value(ret_a + int_b * int_b);
// }

void task1(int a, std::shared_future<int> b, std::promise<int>& p) {
    int ret_a = a * a;
    int int_b = b.get();
    p.set_value(ret_a + int_b * int_b);
}

void task2(int a, std::shared_future<int> b, std::promise<int>& p) {
    int ret_a = a * a;
    int int_b = b.get();
    p.set_value(ret_a + int_b);
}

int main()
{
    std::promise<int> pin;
    std::future<int> fin = pin.get_future(); // 独占，不能被复制只能被移动，因此要引用传递
    std::shared_future<int> sfin = fin.share(); // 可以被复制，因此可以进行值传递。此外，这种future允许多次get()

    std::promise<int> pout1;
    std::future<int> fout1 = pout1.get_future();
    // std::future<int> fout11 = pout1.get_future(); // get_future()也只允许调用一次，第二次会抛出 std::future_error 异常
    std::promise<int> pout2;
    std::future<int> fout2 = pout2.get_future();

    std::thread t1(task1, 2, sfin, std::ref(pout1));
    std::thread t2(task2, 2, sfin, std::ref(pout2));

    std::this_thread::sleep_for(std::chrono::seconds(2));
    pin.set_value(3);
    std::cout << fout1.get() << " " << fout2.get() << std::endl; // f.get()会阻塞直到p得到结果。f.get()只能进行一次，否则会crash
    if (t1.joinable()) t1.join();
    if (t2.joinable()) t2.join();
    return 0;
}
