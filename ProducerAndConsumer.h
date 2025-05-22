#include "SafeQueue.h"

//构造多线程的生产者、消费者，对单一队列进行生产和消费
class ProducerAndConsumer {
public:
    ProducerAndConsumer(): stop(false), capacity(10), m_pnum(1), m_cnum(1) 
    { init(); };
    ProducerAndConsumer(size_t cap, size_t p_num, size_t c_num): 
        stop(false), capacity(cap), m_pnum(p_num), m_cnum(c_num) 
    { init(); }
    ProducerAndConsumer(const ProducerAndConsumer&) = delete;
    ProducerAndConsumer& operator=(const ProducerAndConsumer&) = delete;
    ProducerAndConsumer(ProducerAndConsumer&&) = delete;
    ProducerAndConsumer& operator=(ProducerAndConsumer&&) = delete;
    ~ProducerAndConsumer() {
        { // 先锁住所有线程，确保安全
            std::unique_lock<std::mutex> lock(m_mtx);
            stop.store(true);
        }
        cv_notFull.notify_all();
        cv_notEmpty.notify_all();
        for (auto& t : producers) t.join();
        for (auto& t : consumers) t.join();
    }
private:
    void produce() {
        while (!stop) {
            std::unique_lock<std::mutex> lock(m_mtx);
            cv_notFull.wait(lock, [this]() {
                return stop || Q.size() < capacity;
            });
            if (stop) break; // 如果 stop == true，退出循环
            int val = data++;
            if (data.load() > 100) data.store(0);
            Q.push(val);
            std::cout << "produce: " << val << std::endl;
            cv_notEmpty.notify_one();
        }
    }
    void consume() {
        while (!stop) {
            std::unique_lock<std::mutex> lock(m_mtx);
            cv_notEmpty.wait(lock, [this]() {
                return stop || !Q.empty();
            });
            if (stop) break; // 如果 stop == true，退出循环
            int val = Q.front();
            Q.pop();
            std::cout << "consume: " << val << std::endl;
            cv_notFull.notify_one();
        }
    }
    void init() {
        data.store(0);
        if (m_pnum < 1) m_pnum = 1;
        if (m_cnum < 1) m_cnum = 1;
        for (size_t i=0; i<m_pnum; i++) {
            producers.emplace_back(std::thread(&ProducerAndConsumer::produce, this));
        }
        for (size_t i=0; i<m_cnum; i++) {
            consumers.emplace_back(std::thread(&ProducerAndConsumer::consume, this));
        }
    }
private:
    std::atomic<bool> stop;
    SafeQueue<int> Q; // 也可用 std::queue<int> Q
    std::mutex m_mtx;
    size_t capacity; // 队列容量
    size_t m_pnum, m_cnum; // 生产者、消费者线程数
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    std::condition_variable cv_notFull;
    std::condition_variable cv_notEmpty;
    std::atomic<int> data; // 队列中的模拟数据
};
