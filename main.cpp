#include <iostream>
#include "skip_list.h"
#include "comparator.h"

typedef SkipList<std::string, std::string, KeyComparator> Table;

// SimpleTest For SkipList
void ins(std::shared_ptr<Table> tb) {
    const char *key = "key";
    const char *value = "value";
    auto k1 = std::string(key) + std::to_string(2);
    auto val1 = std::string(value) + std::to_string(2);
    tb->Insert(k1, val1);

    auto k = std::string(key);
    auto val = std::string(value) + std::to_string(1);
    // printf("addr is k: %lx, v: %lx\n", k.c_str(), val.c_str());
    tb->Insert(k, val);
}

int main() {
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
    printf("allocator is %p\n", static_cast<void *>(allocator.get()));
    MyComparator myComparator{};
    KeyComparator c{myComparator};
    printf("before make instance\n");
    auto tb = Table::MakeInstance(c, allocator);
    printf("make instance\n");
    // char *key = "key";
    // char *value = "value";
    // for (int i = 1; i < 100; i++) {
    //     auto k = std::string(key) + std::to_string(i);
    //     auto val = std::string(value) + std::to_string(i);
    //     // printf("k is %lx\n", k2str);
    //     tb->Insert(k.c_str(), val.c_str());
    // }
    // auto k = std::string(key) + std::to_string(1);
    // auto val = std::string(value) + std::to_string(1);
    // tb->Insert(k.c_str(), val.c_str());
    // tb->Insert(key, value);
    ins(tb);
    if (tb->Contains("key")) {
        printf("Success Final\n");    
    }
}