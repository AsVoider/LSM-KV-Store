#include <string>
#include <mystring.h>
#include <memory>

class Comparator 
{
 public:
  virtual ~Comparator() = default;

  virtual auto Compare(const MyString& a, const MyString& b) const -> int = 0;

  virtual auto Name() -> const char * const = 0;

};

class MyComparator : public Comparator 
{
public:
    MyComparator() = default;

    ~MyComparator() override = default;

    auto Name() -> const char * const override;

    auto Compare(const MyString& a, const MyString& b) const -> int override;

    static auto GetInstance() -> std::shared_ptr<MyComparator> const;
};

class KeyComparator
{
public:
    MyComparator myComparator;
    explicit KeyComparator(MyComparator myComparator) : myComparator(myComparator) {
        printf("build heree\n");
    };
    int compare(const char *a, const char *b) const;
    int compare(const std::string &a, const std::string &b) const;
};

// namespace LSM