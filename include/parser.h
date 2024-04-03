#ifndef PARSER
#define PARSER
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

enum class operation{
    PUT,
    GET,
    DEL,
    BATCH_PUT,
    BATCH_GET,
    ERROR,
    DEFAULT
};

template<typename K, typename V>
class Ops {
public:
    operation op;
    std::vector<std::pair<K, V>> vecs;

    Ops& operator=(const Ops&);
    Ops(const Ops& A) = delete;
    Ops(operation oper, std::vector<std::pair<K, V>> &vec);
};

template<typename K, typename V>
auto parse_cmd(std::string &cmd) -> Ops<K, V>;

auto parse_cmd(std::string &cmd) -> Ops<std::string, std::string> {
    std::stringstream strm(cmd);
    std::string token;
    auto vecs = std::vector<std::pair<std::string, std::string>>();
    if (!strm.eof() && (strm >> token, !strm.eof())) {
        
    } else {
        vecs.emplace_back(std::pair<std::string, std::string>("error", "Less Params"));
        return Ops<std::string, std::string>{operation::ERROR, vecs};
    }
    
    if (token == "PUT") {
        if ((strm >> token, token == "KEY") && !strm.eof()) {
            std::string key, value;
            strm >> key;
            if (!strm.eof() && (strm >> token, token == "VALUE") && !strm.eof()) {
                if ((strm >> value, strm.eof())) {
                    std::cout << "end" << std::endl;
                    vecs.emplace_back(std::pair<std::string, std::string>(key, value));
                    return Ops<std::string, std::string>{operation::PUT, vecs};
                } else {
                    vecs.emplace_back(std::pair<std::string, std::string>("error", "Something after your value?"));
                    return Ops<std::string, std::string>{operation::ERROR, vecs};
                }
            } else {
                vecs.emplace_back(std::pair<std::string, std::string>("error", "Where is your \"VALUE\""));
                return Ops<std::string, std::string>{operation::ERROR, vecs};
            }
        } else {
            vecs.emplace_back(std::pair<std::string, std::string>("error", "Where is your \"KEY\""));
            return Ops<std::string, std::string>{operation::ERROR, vecs};
        }
    } else if (token == "GET") {
        if ((strm >> token, token == "KEY") && !strm.eof()) {
            std::string key;
            if ((strm >> key, strm.eof())) {
                vecs.emplace_back(std::pair<std::string, std::string>(key, ""));
                return Ops<std::string, std::string>{operation::GET, vecs};
            } else {
                vecs.emplace_back(std::pair<std::string, std::string>("error", "Something after your key?"));
                return Ops<std::string, std::string>{operation::ERROR, vecs};
            }
        } else {
            vecs.emplace_back(std::pair<std::string, std::string>("error", "Where is your \"KEY\""));
            return Ops<std::string, std::string>{operation::ERROR, vecs};
        }
    } else if (token == "DEL") {
        if ((strm >> token, token == "KEY") && !strm.eof()) {
            std::string key;
            if ((strm >> key, strm.eof())) {
                vecs.emplace_back(std::pair<std::string, std::string>(key, ""));
                return Ops<std::string, std::string>{operation::DEL, vecs};
            } else {
                vecs.emplace_back(std::pair<std::string, std::string>("error", "Something after yout key?"));
                return Ops<std::string, std::string>{operation::ERROR, vecs};
            }
        } else {
            vecs.emplace_back(std::pair<std::string, std::string>("error", "Where is your \"KEY\""));
            return Ops<std::string, std::string>{operation::ERROR, vecs};
        }
    } else if (token == "BATCH_PUT") {
        auto flag = true;
        while(flag == true) {
            if ((strm >> token, token == "KEY") && !strm.eof()) {
                std::string key;
                strm >> key;
                if (!strm.eof() && (strm >> token, token == "VALUE") && !strm.eof()) {
                    std::string value;
                    strm >> value;
                    vecs.emplace_back(std::pair<std::string, std::string>(key, value));
                    if (strm.eof()) {
                        flag = false;
                    } else {
                        flag = true;
                    }
                } else {
                    auto res = std::pair<std::string, std::string>("error", "Where is your \"VALUE\"");
                    std::vector<std::pair<std::string, std::string>> tmp{res};
                    return Ops<std::string, std::string>{operation::ERROR, tmp};
                }
            } else {
                auto res = std::pair<std::string, std::string>("error", "Where is your \"KEY\"");
                std::vector<std::pair<std::string, std::string>> tmp{res};
                return Ops<std::string, std::string>{operation::ERROR, tmp};
            }
        }
        return Ops<std::string, std::string>{operation::BATCH_PUT, vecs};
    } else if (token == "BATCH_GET") {
        if ((strm >> token, token == "KEYS") && !strm.eof()) {
            while (strm >> token) {
                vecs.emplace_back(std::pair<std::string, std::string>(token, ""));
            }
            return Ops<std::string, std::string>{operation::BATCH_GET, vecs};
        } else {
            vecs.emplace_back(std::pair<std::string, std::string>("error", "Where is your \"KEYS\""));
            return Ops<std::string, std::string>{operation::ERROR, vecs};
        }
    } else {
        vecs.emplace_back(std::pair<std::string, std::string>("error", "Please Select From \"PUT\", \"GET\", \"DEL\", \"BATCH_PUT\", \"BATCH_GET\""));
        return Ops<std::string, std::string>(operation::DEFAULT, vecs);
    }
    return Ops<std::string, std::string>{operation::PUT, vecs};
}

template<typename K, typename V>
Ops<K,V>& Ops<K, V>::operator=(const Ops& A) {
    this = &A;
}

template<typename K, typename V>
Ops<K, V>::Ops(operation op, std::vector<std::pair<K, V>> &vec) {
    this->op = op;
    this->vecs = vec;
}

#endif