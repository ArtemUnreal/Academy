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
    void enqueue(int priority, std::function<void()> task);

private:
    struct Task 
    {
        int priority = 0;
        std::function<void()> func;
        Task() = default;
        Task(int p, std::function<void()> f) : priority(p), func(std::move(f)) {}
    };

    struct CompareTask 
    {
        bool operator()(const Task& a, const Task& b) 
        {
            return a.priority < b.priority;
        }
    };

private:
    std::vector<std::thread> _threads;
    std::priority_queue<Task, std::vector<Task>, CompareTask> _tasks;
    std::mutex _queue_mutex;
    std::condition_variable _cv;
    bool _stop = false;
};