#include <string>
#include <mystring.h>
#include <memory>

class Comparator 
{
 public:
  virtual ~Comparator();

  virtual auto Compare(const MyString& a, const MyString& b) -> int const = 0;

  virtual auto Name() -> const char * const = 0;

};

class MyComparator : public Comparator 
{
public:
    MyComparator() = default;

    auto Name() -> const char * const override;

    auto Compare(const MyString& a, const MyString& b) -> int const override;

    static auto GetInstance() -> std::shared_ptr<MyComparator> const;
};

class KeyComparator
{
public:
    MyComparator myComparator;
    explicit KeyComparator(MyComparator myComparator) : myComparator(myComparator) {};
    auto operator()(const char *a, const char *b) -> int;
};

// namespace LSM