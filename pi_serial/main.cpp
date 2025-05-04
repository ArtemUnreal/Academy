/* 
 * File:   pi_serial.c
 * Author: nd159473 (Nickolay Dalmatov, Sun Microsystems)
 * modified by Dmitry V Irtegov, NSU
 *
 * Created on March 20, 2007, 6:33 PM
 *
 */

#include <iostream>
#include <csignal>
#include <chrono>
#include <atomic>
#include <future>
#include <vector>
#include <thread>
 
 /*
  * 
  */


 
const int num_steps =  200000000;
std::atomic<bool> stop_flag(false);

void signal_handler(int)
{
     stop_flag.store(true);
}

void calculate_partial(int start, int step, std::promise<double> part_promise)
{
     double pi = 0;

     for (int i = start; i < num_steps && !stop_flag.load(); i += step)
     {
          pi += 1.0/(i*4.0 + 1.0);
          pi -= 1.0/(i*4.0 + 3.0);
     }

     part_promise.set_value(pi);
}
 
int main(int argc, char** argv) 
{
    signal(SIGINT, signal_handler);

    int count_of_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    std::vector<std::future<double>> futures;

    for (int i = 0; i < count_of_threads; ++i)
    {
          std::promise<double> p;
          futures.push_back(p.get_future());
          threads.emplace_back(calculate_partial, i, count_of_threads, std::move(p));
    }

    while (!stop_flag.load())
    {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));

          bool done = true;
          for (auto& a : futures)
          {
               if (a.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
               {
                    done = false;
                    break;
               }
          }

          if (done) break;
    }

     double sum = 0;

     for (auto& a : futures) sum += a.get();

     for (auto& a : threads)
          if (a.joinable()) a.join();

     std::cout << "Pi: " << sum * 4.0 << std::endl;
    
     return 0;
 }
 