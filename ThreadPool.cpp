#include "ThreadPool.h"

int myAdd(int a, int b) {
    std::cout << " working on task " << a << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    return a + b;
}

int main()
{
    std::cout << "main thread id: " << std::this_thread::get_id() << std::endl;
    size_t task_num = 20;
    std::vector<std::future<int>> results(task_num); // 默认构造的就是空future。
    ThreadPool tp1(4, 4);

    std::cout << "start task enqueue" << std::endl;
    for (size_t i=0; i<task_num; i++) {
        std::cout << "task " << i << " enqueue" << std::endl;
        try { // 增加了错误捕获处理
            results[i] = tp1.submitTask(myAdd, i, i + 1);
        } catch (const std::exception& ex) {
            std::cerr << "Submit failed: " << ex.what() << std::endl;
            break; // 遇到异常说明stop了，直接结束任务装填
        }
    }
    std::cout << "finish task enqueue" << std::endl;

    // std::this_thread::sleep_for(std::chrono::seconds(16)); // 无意义，future::get()本来就会阻塞等待
    std::cout << "start get result" << std::endl;
    for (size_t i=0; i<task_num; i++) {
        if (results[i].valid()) { // 增加了future有效检测
            int tmp_res = results[i].get();
            std::cout << "Task " << i << " returns " << tmp_res << std::endl;
        } else {
            std::cerr << "Task " << i << " submission failed, future invalid." << std::endl;
        }
    }
    std::cout << "finish get result" << std::endl;
    return 0;
}
