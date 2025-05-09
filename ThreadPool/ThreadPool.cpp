#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t num_threads)
{
    for (size_t i = 0; i < num_threads; ++i)
    {
        _threads.emplace_back([this]
        {
            while(true)
            {
                Task task;

                {
                    std::unique_lock<std::mutex> lock(_queue_mutex);
                    _cv.wait(lock, [this] { return !_tasks.empty() || _stop; });

                    if (_stop && _tasks.empty()) return;

                    task = _tasks.top();
                    _tasks.pop();
                }

                task.func();
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

void ThreadPool::enqueue(int priority, std::function<void()> task) 
{
    {
        std::unique_lock<std::mutex> lock(_queue_mutex);
        _tasks.emplace(priority, std::move(task));
    }

    _cv.notify_all();
}

void ThreadPool::enqueue(std::function<void()> task) 
{
    enqueue(0, std::move(task));
}