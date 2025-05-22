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

std::mutex mtx;
std::condition_variable cv;
std::deque<int> Q;

// producer
void produce() {
    int i = 0;
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        Q.push_back(i);
        cv.notify_one();
        if (i < 10000) {
            i++;
        } else {
            i = 0;
        }
    }
}

// consumer
void consume() {
    int data = 0;
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        // 使用 while 防止虚假唤醒
        while (Q.empty()) {
            cv.wait(lock);
        }
        data = Q.front();
        Q.pop_front();
        lock.unlock();  // 尽早释放锁，提高并发效率
        std::cout << "consume data: " << data << std::endl;
    }
}

int main() {
    std::thread t1(consume);
    std::thread t2(consume);
    std::thread t3(produce);
    if (t1.joinable()) t1.join();
    if (t2.joinable()) t2.join();
    if (t3.joinable()) t3.join();
    return 0;
}
