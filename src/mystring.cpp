#include <mystring.h>
#include <cassert>

decltype(auto) MyString::operator[](size_t n) {
    assert(0 < str.size());
    auto ptr = str.data();
    if (n > str.size())
        return ptr[0];
    return ptr[n];
}

decltype(auto) MyString::clear() {
    str.clear();
    assert(str.empty());
}

decltype(auto) MyString::toString() const {
    return str;
}

decltype(auto) MyString::operator==(const MyString& rhs) const {
    return str == rhs.str;
}

decltype(auto) MyString::operator!=(const MyString& rhs) const {
    return str != rhs.str;
}

inline int MyString::compare(const MyString& rhs) const {
    auto len = str.size() < rhs.str.size() ? str.size() : rhs.str.size();
    auto r = memcmp(str.data(), rhs.str.data(), len);
    if (r == 0) {
        if (str.size() < rhs.str.size())
            r = -1;
        else if (str.size() > rhs.str.size())
            r = 1;
    }
    return r;
}