#include "ThreadPool.h"

int myAdd(int a, int b) {
    std::cout << " working on task " << a << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    return a + b;
}

int main()
{
    std::cout << "main thread id: " << std::this_thread::get_id() << std::endl;
    size_t task_num = 20;
    std::vector<std::future<int>> results(task_num);
    ThreadPool tp1(8);

    std::cout << "start task enqueue" << std::endl;
    for (size_t i=0; i<task_num; i++) {
        std::cout << "task " << i << " enqueue" << std::endl;
        results[i] = tp1.submitTask(myAdd, i, i + 1);
    }
    std::cout << "finish task enqueue" << std::endl;

    // std::this_thread::sleep_for(std::chrono::seconds(16)); // 无意义，future::get()本来就会阻塞等待
    std::cout << "start get result" << std::endl;
    for (std::future<int>& res : results) {
        std::cout << res.get() << std::endl;
    }
    std::cout << "finish get result" << std::endl;
    return 0;
}
