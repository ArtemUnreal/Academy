#include "ThreadPool.h"

int main()
{
    ThreadPool pool(4);

    for (int i = 0; i < 5; ++i)
    {
        pool.enqueue([i] {
            std::cout << "Task " << i << " is running on thread " << std::this_thread::get_id() << std::endl;;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        });
    }

    return 0;
}