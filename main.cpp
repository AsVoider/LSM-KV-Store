#include <iostream>
#include "parser.h"


int main() {
    std::string a = "PUT KEY 1 VALUE 2";
    std::getline(std::cin, a);
    // for(auto chars : a) {
    //     printf("%d ", chars);
    // }
    // std::cout << std::endl;
    auto test = parse_cmd(a);
    std::cout << test.vecs.size() << std::endl;
    for (auto it : test.vecs) {
        std::cout << it.first << " " << it.second << std::endl;
    }
}