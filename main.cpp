#include <iostream>
#include "skip_list.h"
#include "comparator.h"


int main() {
    typedef SkipList<const char *, const char *, KeyComparator> Table;
    // std::string a = "PUT KEY 1 VALUE 2";
    // std::getline(std::cin, a);
    // for(auto chars : a) {
    //     printf("%d ", chars);
    // }
    // std::cout << std::endl;
    // auto test = parse_cmd(a);
    // std::cout << test.vecs.size() << std::endl;
    // for (auto it : test.vecs) {
    //     std::cout << it.first << " " << it.second << std::endl;
    // }

    // Mem_Allocator mem_test{};

    // for (auto i = 0; i <= page_size / B1024; i++) {
    //     auto ptr1 = mem_test.Allocate(B1024);
    //     printf("ptr is %lx\n", reinterpret_cast<uint64_t>(ptr1));
    // }
    // auto res = mem_test.Allocate(128);
    // auto res3 = mem_test.Allocate(large_page_size);
    // auto ptr = res3 + 5 * page_size;
    // ptr[0] = 's';
    //printf("res is %lx res2 is %lx, res3 is %lx\n", res, res2, res3);
    // printf("ptr is %lx",  reinterpret_cast<uint64_t>(ptr));
    auto allocator = std::make_shared<Mem_Allocator>();
    printf("allocator is %lx\n", allocator.get());
    MyComparator myComparator{};
    KeyComparator c{myComparator};
    printf("before make instance\n");
    auto tb = Table::MakeInstance(c, allocator);
    printf("make instance\n");
    char *key = "key";
    char *value = "value";
    for (int i = 1; i < 100; i++) {
        auto k = std::string(key) + std::to_string(i);
        auto k2str = k.c_str();
        printf("k is %s i is %d\n", k2str, i);
        tb->Insert(k2str, value);
    }
    tb->Insert(key, value);
}