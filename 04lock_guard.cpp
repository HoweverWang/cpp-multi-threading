#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional> // for std::ref
#include <cmath>
#include <mutex>
#include <thread>
#include <atomic>

std::timed_mutex mutex1;
std::mutex mutex2;

void increment1(int& num) {
    for (int i=0; i<10; i++) {
        std::unique_lock<std::timed_mutex> lg1(mutex1, std::defer_lock);
        if (lg1.try_lock_for(std::chrono::seconds(3))) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            num++;
        } else {
            std::cout << "failed to get lock, continue" << std::endl;
        }
    }
}

void increment2(int& num) {
    for (int i=0; i<10; i++) {
        std::unique_lock<std::timed_mutex> lg1(mutex1, std::defer_lock);
        if (lg1.try_lock_for(std::chrono::seconds(3))) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            num++;
        } else {
            std::cout << "failed to get lock, continue" << std::endl;
        }
    }
}

int main() {
    int num = 0;
    std::thread t1(increment1, std::ref(num));
    std::thread t2(increment2, std::ref(num));
    t1.join();
    t2.join();
    std::cout << num << std::endl;
    return 0;
}
