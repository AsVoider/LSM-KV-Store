#include "comparator.h"

auto MyComparator::Name() -> const char * const {
    return "LSM.MyComparator";
}

auto MyComparator::Compare(const MyString& a, const MyString& b) -> int const {
    return a.compare(b);
}

static auto GetInstance() -> std::shared_ptr<MyComparator> const {
    static auto instance = std::make_shared<MyComparator>();
    return instance;
}

auto KeyComparator::operator()(const char *a, const char *b) -> int {
    return myComparator.Compare(MyString(a), MyString(b));
}