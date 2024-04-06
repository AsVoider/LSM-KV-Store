#include <string>
#include <cstring>

class MyString
{
public:
    MyString() : str("") {};
    MyString(const char* d, size_t n) : str(d, n) {};
    MyString(const std::string& s) : str(s) {};
    MyString(const char* s) : str(s, strlen(s)) {};
    MyString& operator=(const MyString&) = default;
    auto data() const -> const char * { return str.data(); }
    auto size() -> size_t const { return str.size(); }
    auto empty() -> bool const { return str.empty(); }

    decltype(auto) operator[](size_t n);
    decltype(auto) clear();
    decltype(auto) toString() const;
    decltype(auto) operator==(const MyString& rhs) const;
    decltype(auto) operator!=(const MyString& rhs) const;
    inline int compare(const MyString& rhs) const;

private:
    std::string str;
};