#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <iostream>

class ThreadPool
{
public:
    ThreadPool(size_t num_threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    void enqueue(std::function<void()> task);

private:
    std::vector<std::thread> _threads;
    std::queue<std::function<void()>> _tasks;
    std::mutex _queue_mutex;
    std::condition_variable _cv;
    bool _stop = false;
};