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
