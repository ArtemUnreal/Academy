//"How + to + play ping + pong -twice-> ping pong "

#include <iostream>
#include <thread>
#include <string>
#include <sstream>
#include <future>

using namespace std::chrono_literals;

namespace
{
    class SafeCout: public std::stringstream
    {
    public:
        ~SafeCout()
        {
            std::lock_guard<std::mutex> l {coutMutex};
            std::cout << rdbuf();
            std::cout.flush();
        }

    private:
        static std::mutex coutMutex;    
    };

    std::mutex SafeCout::coutMutex;

    std::string CreateString(const char* cStr)
    {
        SafeCout() << "3 sec creation: " << cStr << '\n';
        std::this_thread::sleep_for(3s);
        return {cStr};
    }

    std::string Concat(const std::string& a, const std::string& b)
    {
        SafeCout() << "5 sec concatenation: " << a << " " << b << '\n';
        std::this_thread::sleep_for(5s);
        return a + b;
    }

    std::string Twice(const std::string& s)
    {
        SafeCout() << "2 sec twice: " << s << " " << s << '\n';
        std::this_thread::sleep_for(2s);
        return s + s;
    }

    template<typename F>
    auto AsyncFn(F fn)
    {
        return [fn](auto... args)
        {
            return [=]()
            {
                return std::async(std::launch::async, fn, args...);
            };
        };
    }

    template<typename F>
    auto FutureUnwrap(F fn)
    {
        return [fn](auto... futures)
        {
            return fn(futures.get()...);
        };
    }

    template<typename F>
    auto AsyncAdapter(F fn)
    {
        return [fn](auto... args)
        {
            return [=]()
            {
                return std::async(std::launch::async, FutureUnwrap(fn), args()...);
            };
        };
    }
}

int main()
{
    auto asyncCreateString(AsyncFn(CreateString));
    auto asyncConcat(AsyncAdapter(Concat));
    auto asyncTwice(AsyncAdapter(Twice));

    auto result (asyncConcat(
                    asyncConcat(
                        asyncConcat(asyncCreateString("how"), asyncCreateString("to")),
                        asyncCreateString("play")
                    ),
                    asyncTwice(
                        asyncConcat(asyncCreateString("ping"), asyncCreateString("pong"))
                    )
                )
                );
                
    std::cout << result().get() << std::endl;
}