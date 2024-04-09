#include "comparator.h"

auto MyComparator::Name() -> const char * const {
    return "LSM.MyComparator";
}

auto MyComparator::Compare(const MyString& a, const MyString& b) const -> int {
    return a.compare(b);
}

static auto GetInstance() -> std::shared_ptr<MyComparator> const {
    static auto instance = std::make_shared<MyComparator>();
    return instance;
}

int KeyComparator::compare(const char *a, const char *b) const {
    return myComparator.Compare(MyString(a), MyString(b));
}