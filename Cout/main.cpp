#include <iostream>
#include <string>

class NewLineInserter 
{
public:
    explicit NewLineInserter(std::ostream& os) : wrapped(os) 
    {}

    template <typename T>
    NewLineInserter& operator<<(const T& value) 
    {
        wrapped << value;
        return *this;
    }

    NewLineInserter& operator<<(std::ostream& (*func)(std::ostream&)) 
    {
        func(wrapped);  
        wrapped << '\n'; 
        return *this;
    }

private:
    std::ostream& wrapped;
};

int main() {
    NewLineInserter c_out(std::cout);
    c_out << "Hello, world!" << " This is a test. " << std::flush;
    c_out << 42 << std::flush;
    return 0;
}
