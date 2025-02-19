#include <string>
#include <iostream>

class MyStream : public std::ostream
{
public:
    MyStream(std::ostream& os) : stream(os) { }
    ~MyStream() { stream << "\n"; }
    
    template<typename T>
    MyStream& operator<<(const T& value)
    {
        stream << value;
        return *this;
    }

private:
    std::ostream& stream;
};

class MyCout : public std::ostream
{
public:
    MyCout() : std::ostream(std::cout.rdbuf()) { }

    template <typename T>
    MyStream operator<<(const T& value)
    {
        static_cast<std::ostream&>(*this) << value;
        
        return MyStream(*this);
    }
};

int main()
{
    MyCout my;
    my << 43;
}
