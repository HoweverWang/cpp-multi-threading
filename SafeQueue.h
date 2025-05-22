#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <functional> // for std::ref
#include <cmath>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <future>

// 一个简单的线程安全队列，支持泛型编程
// 没有容量管理，因此需要使用者自行处理容量空或满
template<typename T>
class SafeQueue {
public:
    SafeQueue() = default;
    ~SafeQueue() = default;
    SafeQueue(const SafeQueue& other) = delete; // 禁止拷贝构造
    SafeQueue& operator=(const SafeQueue& other) = delete; // 禁止拷贝赋值
    SafeQueue(SafeQueue&& other) = delete; // 禁止移动构造
    SafeQueue& operator=(SafeQueue&& other) = delete; // 禁止移动赋值
    bool empty() {
        // lock_guard比unique_lock更轻量级
        std::lock_guard<std::mutex> lock(m_mtx);
        return Q.empty();
    }
    size_t size() {
        std::lock_guard<std::mutex> lock(m_mtx);
        return Q.size();
    }
    void push(const T& val) {
        std::lock_guard<std::mutex> lock(m_mtx);
        Q.push(val);
    }
    void push(T&& val) {
        std::lock_guard<std::mutex> lock(m_mtx);
        Q.push(std::move(val));
    }
    template<typename... Args>
    void emplace(Args&&... args) {
        std::lock_guard<std::mutex> lock(m_mtx);
        Q.emplace(std::forward<Args>(args)...);
    }
    T pop() {
        std::lock_guard<std::mutex> lock(m_mtx);
        if (Q.empty()) throw std::runtime_error("pop from empty queue");
        T tmp = std::move(Q.front());
        Q.pop();
        return tmp;
    }
    T front() {
        std::lock_guard<std::mutex> lock(m_mtx);
        if (Q.empty()) throw std::runtime_error("front from empty queue");
        return Q.front();
    }
    T back() {
        std::lock_guard<std::mutex> lock(m_mtx);
        if (Q.empty()) throw std::runtime_error("back from empty queue");
        return Q.back();
    }
private:
    std::queue<T> Q;
    std::mutex m_mtx;
};
