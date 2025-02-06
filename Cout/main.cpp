#include <iostream>
#include <streambuf>

class LineBreak { };

class LineBreakProxy 
{
public:
    std::ostream& os;
    explicit LineBreakProxy(std::ostream& os_) : os(os_) { }
    
    template <typename T>
    LineBreakProxy& operator<<(const T& value) 
    {
        os << value;
        return *this;
    }
    
    typedef std::ostream& (*OStreamManipulator)(std::ostream&);
    LineBreakProxy& operator<<(OStreamManipulator manip) 
    {
        manip(os);
        return *this;
    }
};

LineBreakProxy operator<<(std::ostream& os, const LineBreak&) 
{
    os.put(os.widen(10));
    os.flush();
    return LineBreakProxy(os);
}

int main() {
    std::cout << "First" << LineBreak() 
              << "Second" << LineBreak() 
              << 200 << LineBreak();
    return 0;
}
