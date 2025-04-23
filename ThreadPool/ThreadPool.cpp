#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t num_threads)
{
    for (size_t i = 0; i < num_threads; ++i)
    {
        _threads.emplace_back([this]
        {
            while(true)
            {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(_queue_mutex);
                    _cv.wait(lock, [this] { return !_tasks.empty() || _stop; });

                    if (_stop && _tasks.empty()) return;

                    task = std::move(_tasks.front());
                    _tasks.pop();
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _stop = true;
    }

    _cv.notify_all();

    for (auto& thread : _threads)
    {
        thread.join();
    }
}

void ThreadPool::enqueue(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _tasks.emplace(std::move(task));
    }

    _cv.notify_all();
}