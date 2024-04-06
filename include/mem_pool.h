#include <atomic>
#include <vector>
#include <iostream>
#include <memory>

constexpr uint32_t B32 = 32;
constexpr uint32_t B64 = 64;
constexpr uint32_t B128 = 128;
constexpr uint32_t B256 = 256;
constexpr uint32_t B512 = 512;
constexpr uint32_t B1024 = 1024;
constexpr uint32_t page_size = 4096;
constexpr uint32_t large_page_size = 2097152;
constexpr uint32_t freenum_32B = page_size / 32;
constexpr uint32_t freenum_64B = page_size / 64;
constexpr uint32_t freenum_128B = page_size / 128;
constexpr uint32_t freenum_256B = page_size / 256;
constexpr uint32_t freenum_512B = page_size / 512;
constexpr uint32_t freenum_1kB = page_size / 1024;

enum block_type : uint32_t 
{
    SIZE_32,
    SIZE_64,
    SIZE_128,
    SIZE_256,
    SIZE_512,
    SIZE_1024,
    TOTAL_PAGE,
    SIZE_2M,
    ERROR
};

struct block_ptr
{
    block_type this_type;
    uint32_t free_block_num;
    char *this_ptr;
    char *now_ptr;

    block_ptr(block_type type, uint32_t this_free, char *ptr) {
        this_type = type;
        free_block_num = this_free;
        this_ptr = ptr;
        now_ptr = ptr;
    }
};



class Mem_Allocator
{
public:
    Mem_Allocator();
    Mem_Allocator& operator=(const Mem_Allocator &) = delete;
    Mem_Allocator(const Mem_Allocator &) = delete;
    ~Mem_Allocator();

    auto Allocate(uint32_t) -> char *;
    auto Mem_Used() const -> uint32_t;
    static auto GetInstance() -> std::shared_ptr<Mem_Allocator> const;

private:
    std::vector<std::vector<block_ptr>> mem_pool;
    std::atomic<uint32_t> mem_usage;

    auto allocate_newblock(block_type) -> bool;
    auto allocate_in_exist_page(block_type) -> char *;
};