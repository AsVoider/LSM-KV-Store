#include "mem_pool.h"

mem_allocator::mem_allocator() : mem_usage(0) {
    mem_pool.reserve(8);
    for (auto i = 0; i < 8; i++) {
        mem_pool.emplace_back(std::vector<block_ptr>());
    }
}

mem_allocator::~mem_allocator() {
    std::cout << "~mem_allocator now" << std::endl;
    for (auto &vec : mem_pool) {
        for (auto &blk_ptr : vec) {
            delete[] blk_ptr.this_ptr;
        }
        vec.clear();
    }
    mem_pool.clear();
}

auto mem_allocator::mem_used() const -> uint32_t {
    return this->mem_usage.load(std::memory_order_relaxed);
}

auto mem_allocator::allocate_newblock(block_type type) -> bool {
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
    return true;
}

auto mem_allocator::allocate_in_exist_page(block_type type) -> char *{
    if (type >= block_type::TOTAL_PAGE) {
        return nullptr;
    }

    auto &back_page = mem_pool[type].back();

    if (back_page.free_block_num == 0) {
        return nullptr;
    }

    back_page.free_block_num--;
    auto return_ptr = back_page.now_ptr;
    back_page.now_ptr += (type + 1) * 32;
    return return_ptr;
}

auto mem_allocator::Allocate(uint32_t size) -> char * {
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