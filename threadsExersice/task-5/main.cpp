#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include <iterator>
#include "tests.h"

template <typename T>
class ConcurrentFIFOQueue {
public:
    // добавлен лимит на размер очереди
    explicit ConcurrentFIFOQueue(size_t limit = 0) : _limit(limit) {}

    template <typename InputIt>
    size_t push_bulk(InputIt in_iter, int max_count)
    {
        std::unique_lock<std::mutex> l(_m);

        size_t count_of_success_add_elem = 0;
    
        if (_limit && _queue.size() == _limit) 
        {
            _not_limit_cv.wait(l, [this]{ return _queue.size() < _limit; });
        }
    
        while (count_of_success_add_elem < max_count && (_limit == 0 || _queue.size() < _limit))
        {
            _queue.push(*in_iter);
            ++in_iter;
            ++count_of_success_add_elem;
        }

        if (count_of_success_add_elem > 0)
        {
            _not_empty_cv.notify_all(); 
        }

        return count_of_success_add_elem;
    }

    void push(const T& val) 
    {
        std::unique_lock l(_m);
        
        if (_limit)
        {
            if (_queue.size() == _limit)
            {
                _not_limit_cv.wait(l, [this] { return _queue.size() != _limit; });
            }
        }
        
        _queue.push(val);
        if (_queue.size() == 1)
        {
            _not_empty_cv.notify_all();
        }
    }

    template<typename OutputIt>
    size_t pop_bulk(OutputIt out_iter, int max_count)
    {
        std::unique_lock<std::mutex> l(_m);

        _not_empty_cv.wait(l, [this] { return !_queue.empty(); });
        
        size_t count_of_success_of_removed_element = 0;

        while (count_of_success_of_removed_element < max_count && !_queue.empty())
        {
            *out_iter = _queue.front();
            _queue.pop();
            ++out_iter;
            ++count_of_success_of_removed_element;
        }

        _not_limit_cv.notify_all();
        return count_of_success_of_removed_element;
    }

    T pop() {
        std::unique_lock l(_m);
        if (_queue.size() == 0)
        {
            _not_empty_cv.wait(l, [this] { return _queue.size() != 0; });
        }
        auto value = _queue.front();
        _queue.pop();
        
        if (_limit)
        {
            if (_queue.size() == (_limit - 1))
            {
                _not_limit_cv.notify_all();
            }
        }
        return value;
    }

    bool try_push(const T& val) 
    {
        std::lock_guard<std::mutex> lock(_m);
        
        if (_limit > 0 && _queue.size() >= _limit) return false;
        
        _queue.push(val);
        _not_empty_cv.notify_one();

        return true;
    }

    bool try_pop(T& val) 
    {
        std::lock_guard<std::mutex> lock(_m);
        
        if (_queue.empty()) return false;
        
        val = _queue.front();
        _queue.pop();
        _not_limit_cv.notify_one();

        return true;
    }

private:
    std::mutex _m;
    std::condition_variable _not_empty_cv;
    std::condition_variable _not_limit_cv;
    
    std::queue<T> _queue;
    size_t _limit;
};

/*
 * Тесты
 */
void test_multiple_push_pop() {
    ConcurrentFIFOQueue<int> queue;

    queue.push(1);
    queue.push(2);
    queue.push(3);
    EXPECT(queue.pop() == 1);
    EXPECT(queue.pop() == 2);
    EXPECT(queue.pop() == 3);

    PASS();
}

void test_pop_wait() {
    ConcurrentFIFOQueue<int> queue;
    std::atomic<bool> item_popped{false};

    std::thread consumer{[&]() {
        queue.pop();
        item_popped.store(true);
    }};

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT(item_popped.load() == false);

    queue.push(1);

    consumer.join();

    EXPECT(item_popped.load() == true);

    PASS();
}

void test_push_wait() {
    // Проверяем, что push блокируется, если очередь переполнена
    constexpr auto Limit = 2u;
    ConcurrentFIFOQueue<int> queue{Limit};

    std::atomic_int values_pushed{0};

    std::thread producer([&]() {
        for (int i = 0; i < Limit + 1; ++i) {
            queue.push(i);
            values_pushed++;
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    // Добавлено в очередь `Limit` элементов, и один `push` в ожидании
    EXPECT(values_pushed.load() == Limit);

    // Разблокируем оставшийся поток
    queue.pop();
    producer.join();

    EXPECT(values_pushed.load() == Limit + 1);

    PASS();
}

void test_multiple_threads() {
    constexpr auto NumThreads = 4;
    constexpr auto N = 100;  // каждый producer поток производит N чисел

    ConcurrentFIFOQueue<int> queue{2};  // лимит в 2 элемента

    std::vector<int> consumed;
    std::mutex consumed_mutex;

    auto producer_func = [&](int thread_num) {
        for (int i = 0; i < N; ++i) {
            int num = thread_num * N + i;
            queue.push(num);
        }
    };

    auto consumer_func = [&]() {
        for (int i = 0; i < N; ++i) {
            int num = queue.pop();

            std::lock_guard<std::mutex> lock(consumed_mutex);
            consumed.push_back(num);
        }
    };

    std::vector<std::thread> threads;

    for (int i = 0; i < NumThreads; ++i) {
        threads.emplace_back(producer_func, i);
        threads.emplace_back(consumer_func);
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT(consumed.size() == N * NumThreads);

    std::sort(std::begin(consumed), std::end(consumed));

    for (int i = 1; i < N; ++i) {
        EXPECT(consumed[i] == consumed[i - 1] + 1);
    }

    PASS();
}

void test_bulk_basic() 
{
    ConcurrentFIFOQueue<int> queue;

    std::vector<int> input = {1, 2, 3};
    size_t pushed = queue.push_bulk(input.begin(), 3);
    EXPECT(pushed == 3);

    std::vector<int> output(3, 0);
    size_t popped = queue.pop_bulk(output.begin(), 3);
    EXPECT(popped == 3);
    EXPECT(output == input);

    PASS();
}

void test_push_bulk_partial() 
{
    constexpr auto Limit = 3;
    ConcurrentFIFOQueue<int> queue{Limit};

    std::vector<int> input = {1, 2, 3, 4, 5};
    size_t pushed = queue.push_bulk(input.begin(), 5);
    EXPECT(pushed == 3); 

    std::vector<int> output(3, 0);
    queue.pop_bulk(output.begin(), 3);

    pushed = queue.push_bulk(input.begin() + 3, 2);
    EXPECT(pushed == 2);

    PASS();
}

void test_pop_bulk_partial() {
    ConcurrentFIFOQueue<int> queue;

    std::vector<int> v = {1, 2};
    queue.push_bulk(v.begin(), 2);

    std::vector<int> output(5, 0);
    size_t popped = queue.pop_bulk(output.begin(), 5);
    EXPECT(popped == 2); 

    PASS();
}

void test_try_push_pop() 
{
    ConcurrentFIFOQueue<int> queue(2);

    // Try push пока не заполнится
    EXPECT(queue.try_push(1) == true);
    EXPECT(queue.try_push(2) == true);
    EXPECT(queue.try_push(3) == false); // лимит достигнут

    // Try pop пока не опустеет
    int val;
    EXPECT(queue.try_pop(val) == true && val == 1);
    EXPECT(queue.try_pop(val) == true && val == 2);
    EXPECT(queue.try_pop(val) == false); // очередь пуста

    PASS();
}

int main() {
    try {
        test_multiple_push_pop();
        test_pop_wait();
        test_push_wait();
        test_multiple_threads();
        test_bulk_basic();
        test_push_bulk_partial();
        test_pop_bulk_partial();
        test_try_push_pop();

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
/* Усложнения:
 * - добавь метод `size_t push_bulk(in_iter, max_count)`, позволяющий добавить несколько элементов за один вызов,
 *   и возвращающий количество успешно добавленных элементов. Метод так же ждёт в случае переполненной очереди,
 *   после чего пытается вставить максимально возможное в данный момент времени количество элементов.
 *
 * - добавь аналогичный метод `size_t pop_bulk(out_iter, max_count)`.
 * */