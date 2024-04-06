#include "comparator.h"

auto MyComparator::Name() -> const char * const {
    return "LSM.MyComparator";
}

auto MyComparator::Compare(const MyString& a, const MyString& b) -> int const {
    return a.compare(b);
}

auto MyComparator::GetInstance() -> const std::shared_ptr<MyComparator> {
    return std::shared_ptr<MyComparator>();
}

auto KeyComparator::operator()(const char *a, const char *b) -> int {
    return myComparator.Compare(MyString(a), MyString(b));
}