#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional> // for std::ref
#include <cmath>
#include <mutex>
#include <thread>
#include <atomic>

class MyClass {
public:
    void func() {
        std::cout << "Thread " << std::this_thread::get_id()
                  << " started" << std::endl;
        for (int i=0; i<999; i++) {i++;}
        std::cout << "Thread " << std::this_thread::get_id()
                  << " finished" << std::endl;
    }
};

int main() {
    std::cout << "Thread " << std::this_thread::get_id()
              << " started" << std::endl;

    MyClass obj;
    std::thread t(&MyClass::func, &obj/*this指针*/);
    t.join();  // 等待线程结束

    std::cout << "Thread " << std::this_thread::get_id()
              << " finished" << std::endl;
    return 0;
}
