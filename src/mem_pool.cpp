#include "mem_pool.h"
#include <cmath>

Mem_Allocator::Mem_Allocator() : mem_usage(0) {
    mem_pool.reserve(8);
    for (auto i = 0; i < 8; i++) {
        mem_pool.emplace_back(std::vector<block_ptr>());
    }
    printf("init success\n");
}

Mem_Allocator::~Mem_Allocator() {
    std::cout << "~Mem_Allocator now" << std::endl;
    for (auto &vec : mem_pool) {
        for (auto &blk_ptr : vec) {
            delete[] blk_ptr.this_ptr;
        }
        vec.clear();
    }
    mem_pool.clear();
}

auto Mem_Allocator::Mem_Used() const -> uint32_t {
    return this->mem_usage.load(std::memory_order_relaxed);
}

auto Mem_Allocator::allocate_newblock(block_type type) -> bool {
    char *blk_ptr = nullptr;
    uint32_t free_block_num;
    switch (type) {
        case block_type::SIZE_32:
            free_block_num = freenum_32B;
            blk_ptr = new char[page_size];
            break;
        case block_type::SIZE_64:
            free_block_num = freenum_64B;
            blk_ptr = new char[page_size];
            break;
        case block_type::SIZE_128:
            free_block_num = freenum_128B;
            blk_ptr = new char[page_size];
            break;
        case block_type::SIZE_256:
            free_block_num = freenum_256B;
            blk_ptr = new char[page_size];
            break;
        case block_type::SIZE_512:
            free_block_num = freenum_512B;
            blk_ptr = new char[page_size];
            break;
        case block_type::SIZE_1024:
            free_block_num = freenum_1kB;
            blk_ptr = new char[page_size];
            break;
        case block_type::TOTAL_PAGE:
            free_block_num = 1;
            blk_ptr = new char[page_size];
            break;
        case block_type::SIZE_2M:
            free_block_num = 1;
            blk_ptr = new char[large_page_size];
            break;
        default:
            return false;
    }

    block_ptr new_ptr(type, free_block_num, blk_ptr);
    mem_pool[type].emplace_back(new_ptr);
    mem_usage.fetch_add((type > block_type::TOTAL_PAGE ? large_page_size : page_size) 
        + sizeof(block_ptr), std::memory_order_relaxed);
    return true;
}

auto Mem_Allocator::allocate_in_exist_page(block_type type) -> char *{
    if (type >= block_type::TOTAL_PAGE) {
        return nullptr;
    }

    if (mem_pool[type].size() == 0) {
        return nullptr;
    }

    auto &back_page = mem_pool[type].back();
    
    if (back_page.free_block_num == 0) {
        return nullptr;
    }
    printf("here??\n");
    back_page.free_block_num--;
    auto return_ptr = back_page.now_ptr;
    printf("allocate is %d\n", 32 * static_cast<uint32_t>(pow(2, type)));
    back_page.now_ptr += 32 * static_cast<uint32_t>(pow(2, type));
    return return_ptr;
}

auto Mem_Allocator::Allocate(uint32_t size) -> char * {
    if (size > large_page_size) {
        return nullptr;
    }

    if (size > page_size) {
        if (allocate_newblock(block_type::SIZE_2M) == false) {
            return nullptr;
        } else {
            return mem_pool[block_type::SIZE_2M].back().this_ptr;
        }
    }

    if (size > B1024) {
        if (allocate_newblock(block_type::TOTAL_PAGE) == false) {
            return nullptr;
        } else {
            return mem_pool[block_type::TOTAL_PAGE].back().this_ptr;
        }
    }

    auto type = block_type::ERROR;
    if (size > B512) {
        type = block_type::SIZE_1024;
    } else if (size > B256) {
        type = block_type::SIZE_512;
    } else if (size > B128) {
        type = block_type::SIZE_256;
    } else if (size > B64) {
        type = block_type::SIZE_128;
    } else if (size > B32) {
        type = block_type::SIZE_64;
    } else {
        type = block_type::SIZE_32;
    }
    
    printf("into exit page\n");
    auto return_ptr = allocate_in_exist_page(type);
    if (return_ptr == nullptr) {
        if (allocate_newblock(type) == false) {
            return nullptr;
        } else {
            return allocate_in_exist_page(type);
        }
    } else {
        return return_ptr;
    }
}

auto Mem_Allocator::GetInstance() -> std::shared_ptr<Mem_Allocator> const {
    return std::make_shared<Mem_Allocator>();
}