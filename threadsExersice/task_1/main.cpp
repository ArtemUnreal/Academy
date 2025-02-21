#include <atomic>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

/*
Рабочий код, но нагружает CPU даже в ожидании.
Необходимо переписать с использованием condition_variable.
*/

std::atomic_bool resume{false};
std::mutex name;
std::condition_variable cv;

void waiting_thread_func() {
    std::unique_lock ul(name); // главный поток работает, t поток заблокирован
    std::cout << "another: waiting..." << std::endl;

    cv.wait(ul); // главный поток работает, t ожидает

    std::cout << "another: got the signal!" << std::endl; // t поток заработал
}

int main() {
    std::thread t{waiting_thread_func}; // оба потока активны

    {
        std::lock_guard _(name); // главный поток заблокирван, t поток ожидает
        std::cout << "main: waiting 3 sec..." << std::endl;
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(3)); // главный поток спит, t поток ожидает
    
    std::cout << "main: signaling the other thread to resume" << std::endl; // главный поток работает,  t ождиает

    cv.notify_one(); // главый поток работает, t поток тоже работает

    t.join(); // t поток уничтожается, main поток работает
    return 0;
}
