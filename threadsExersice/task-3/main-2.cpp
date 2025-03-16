#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>
#include <vector>
#include <atomic>
#include "tests.h"

using namespace std::chrono_literals;

class Latch {
public:
    explicit Latch(int64_t threads_expected)
        : threshold_(threads_expected),
          count_(threads_expected),
          generation_(0) {}

    void arrive_and_wait() 
    {
        std::unique_lock lock(mutex_);
        auto current_gen = generation_;

        if (--count_ == 0) 
        {
            count_ = threshold_;
            generation_++;
            cv_.notify_all();
        } 
        else 
        {
            cv_.wait(lock, [this, current_gen] { return generation_ != current_gen; });
        }
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int64_t threshold_;
    int64_t count_;
    int64_t generation_;
};

/*
 * Тесты
 */
void test_latch_synchronizes_threads() {
    constexpr auto num_threads = 16;

    Latch latch{num_threads};

    auto worker = [&]() { latch.arrive_and_wait(); };

    std::vector<std::thread> threads;
    for (auto i = 0u; i < num_threads; i++) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) {
        t.join();
    }
    PASS();
}

void test_latch_awaits() {
    Latch latch{3};

    auto worker = [&]() {
        auto start = std::chrono::steady_clock::now();
        latch.arrive_and_wait();

        auto end = std::chrono::steady_clock::now();
        EXPECT(end - start >= 100ms);
    };

    std::thread t1{worker};
    std::thread t2{worker};

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    latch.arrive_and_wait();

    t1.join();
    t2.join();
    PASS();
}

void test_latch_doesnt_reset() {
    Latch latch{2};

    auto func = [&]() { latch.arrive_and_wait(); };
    std::thread t1{func};
    std::thread t2{func};
    t1.join();
    t2.join();

    std::thread t3{[&]() { latch.arrive_and_wait(); }};
    t3.join();
    PASS();
}

int main() {
    try {
        test_latch_synchronizes_threads();
        test_latch_awaits();

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    test_latch_awaits();
    test_latch_awaits();

    return 0;
}