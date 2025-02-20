#include <string>
#include <iostream>

class MyStream
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

class MyCout 
{
public:
    MyCout(std::ostream& os) : stream(os){ }

    template <typename T>
    MyStream operator<<(const T& value)
    {
        MyStream ms(stream);
        ms << value;
        return ms;
    }
private:
    std::ostream& stream;
};

int main()
{
    MyCout my(std::cout);
    my << 43 << "HELLO";
}
