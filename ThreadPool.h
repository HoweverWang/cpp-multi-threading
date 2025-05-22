/*
单任务队列，任务可以是: 
函数 仿函数 lambda 成员函数 std::function std::packaged_task。
这些任务具有不同的参数列表和返回值，这是一个挑战，需要通用性封装来解决。
*/
#include "SafeQueue.h"

class ThreadPool {
public:
    ThreadPool(): m_stop(false), m_capacity(30) 
    { init(std::thread::hardware_concurrency()); }
    ThreadPool(size_t t_num, size_t cap): m_stop(false), m_capacity(cap) 
    { init(t_num); }
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;
    ~ThreadPool() {
        { // 先锁住所有线程，确保安全
            std::unique_lock<std::mutex> lock(m_mtx);
            m_stop.store(true);
        }
        cv_notFull.notify_all();
        cv_notEmpty.notify_all();
        for (auto& t : m_threads) t.join();
    }
    template<typename Func, typename... Args>
    auto submitTask(Func&& func, Args&&... args) {
        // using returnType = decltype(func(args...)); // 获得返回值类型
        using returnType = typename std::invoke_result_t<Func, Args...>; // 获得返回值类型，更先进的写法
        std::function<returnType()> argsWrapper = std::bind(std::forward<Func>(func), std::forward<Args>(args)...); // 封装传入参数
        auto task = std::make_shared<std::packaged_task<returnType()>>(argsWrapper); // 封装返回值为future，并将task转为共享指针进行持有
        std::future<returnType> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            cv_notFull.wait(lock, [this]() {
                return m_stop || Q.size() < m_capacity;
            });
            if (m_stop) throw std::runtime_error("enqueue on stopped ThreadPool");
            else Q.emplace([task]() { (*task)(); }); // 之前task用了共享指针，从而避免脱离作用域被析构后 Q里的task失效
        }
        cv_notEmpty.notify_one();
        return res;
    }
private:
    void init(size_t t_mun) {
        for (size_t i=0; i<t_mun; i++) {
            auto worker = [this, i]() { // worker会循环读取队列中的task来执行
                while (!m_stop) {
                    TaskType task;
                    {
                        std::unique_lock<std::mutex> lock(m_mtx);
                        cv_notEmpty.wait(lock, [this]() {
                            return m_stop || !Q.empty();
                        });
                        if (m_stop && Q.empty()) break; // 如果stop且队列空了，退出循环
                        std::cout << "worker " << i << ", thread id " << std::this_thread::get_id();
                        task = Q.pop();
                        cv_notFull.notify_one();
                    }
                    task(); // task调用一定记得放到lock之外！否则会变成串行
                }
            };
            m_threads.push_back(std::thread(worker));
        }
    }
private:
    using TaskType = std::function<void()>; // task均被封装为无参数无返回值的function
    std::atomic<bool> m_stop;
    SafeQueue<TaskType> Q;
    std::vector<std::thread> m_threads;
    size_t m_capacity; // 队列容量
    std::mutex m_mtx;
    std::condition_variable cv_notFull;
    std::condition_variable cv_notEmpty;
};
