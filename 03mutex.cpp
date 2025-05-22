#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional> // for std::ref
#include <cmath>
#include <mutex>
#include <thread>
#include <atomic>

std::mutex mutex1;
std::mutex mutex2;

void increment1(int& num) {
    for (int i=0; i<1000000; i++) {
        mutex1.lock();
        mutex2.lock();
        num++;
        mutex2.unlock();
        mutex1.unlock();
    }
}

void increment2(int& num) {
    for (int i=0; i<1000000; i++) {
        mutex1.lock();
        mutex2.lock();
        num++;
        mutex2.unlock();
        mutex1.unlock();
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
