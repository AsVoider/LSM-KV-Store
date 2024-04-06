#include <iostream>
#include "parser.h"
#include "mem_pool.h"


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

    mem_allocator mem_test{};

    for (auto i = 0; i <= page_size / B1024; i++) {
        auto ptr1 = mem_test.Allocate(B1024);
        printf("ptr is %lx\n", reinterpret_cast<uint64_t>(ptr1));
    }
    auto res = mem_test.Allocate(128);
    auto res3 = mem_test.Allocate(large_page_size);
    auto ptr = res3 + 5 * page_size;
    ptr[0] = 's';
    //printf("res is %lx res2 is %lx, res3 is %lx\n", res, res2, res3);
    printf("ptr is %lx",  reinterpret_cast<uint64_t>(ptr));
}