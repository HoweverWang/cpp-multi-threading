#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional> // for std::ref
#include <cmath>
#include <mutex>
#include <thread>
#include <atomic>

// Singleton 类
class Singleton {
private:
    // 私有构造函数，防止外部直接创建实例
    Singleton() {
        std::cout << "Singleton instance created.\n";
    }
    // 静态指针，保存唯一的实例
    inline static Singleton* instance = nullptr;
    // once_flag 用于确保只调用一次初始化
    inline static std::once_flag initFlag;
    
    // 差劲的方案：加锁保护cnt
    // std::mutex mtx;
    // 原子变量
    std::atomic<int> cnt = 0;
public:
    // 禁止拷贝构造和赋值操作符，确保唯一性
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    // 提供全局访问点
    static Singleton* getInstance() {
        std::call_once(initFlag, []() {
            instance = new Singleton();
        });
        return instance;
    }
    // 业务方法
    void showMessage() {
        for (int i=0; i<1000000; i++) {
            cnt++;
        }
        std::cout << "Hello cnt:" << cnt << " from Singleton!" << std::endl;
    }
    // 清理静态实例（可选）
    static void cleanup() {
        delete instance;
        instance = nullptr;
        // 注意：once_flag 不能被重置，标准中没有提供重置方式
        // 所以 cleanup 后不能再 getInstance 创建新实例
    }
};

void thread_func1() {
    // 再次获取单例实例
    Singleton::getInstance()->showMessage();
}

int main() {
    // 获取单例实例并调用方法
    Singleton* singleton1 = Singleton::getInstance();
    singleton1->showMessage();
    // 两个线程也调用方法
    std::thread t1(thread_func1);
    std::thread t2(thread_func1);
    if (t1.joinable()) t1.join();
    if (t2.joinable()) t2.join();

    // 清理单例（如果需要）
    Singleton::cleanup();
    return 0;
}
