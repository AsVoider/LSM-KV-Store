#include <atomic>
#include <cstdlib>
#include <cassert>

#include "mem_pool.h"
#include "random.h"

template<typename Key, typename Value>
struct Node;

template<typename Key, typename Value, class Comparator>
class SkipList;

template<typename Key, typename Value, class Comparator>
class Iterator;

template<typename Key, typename Value>
struct Node{
    Key const key;
    Value const Value;
    std::atomic<Node*> next[1];

    explicit Node(const Key& k, const Value& v) : Key(k), Value(v) {};

    auto Next(int n) -> Node * {
        return next[n].load(std::memory_order_acquire);
    }

    auto SetNext(int n, Node* x) -> Node * {
        next[n].store(x, std::memory_order_release);
    }

    auto NoBarrier_Next(int n) -> Node * {
        assert(n >= 0);
        return next_[n].load(std::memory_order_relaxed);
    }

    auto NoBarrier_SetNext(int n, Node* x) -> void {
        assert(n >= 0);
        next_[n].store(x, std::memory_order_relaxed);
    }
};

template<typename KEY, typename VALUE, class Comparator>
class SkipList 
{
public:
    explicit SkipList(Comparator cmp, std::shared_ptr<Mem_Allocator> mem);
    SkipList(const SkipList &) = delete;
    SkipList& operator=(const SkipList &) = delete;
    static auto MakeInstance(Comparator cmp, std::shared_ptr<Mem_Allocator> mem) -> std::shared_ptr<SkipList> const;
    
    auto Insert(const KEY &key, const VALUE &value) -> void;
    auto Contains(const KEY &key) -> bool;

private:
    int maxHeight = 8;
    Random rnd;
    Comparator const comparator;
    std::shared_ptr<Mem_Allocator> allocator;
    std::atomic<int> max_height;
    Node *const head;
    
    auto getMaxHeight() -> int const;
    auto NewNode(const KEY& k, const VALUE& v, int height) -> Node<KEY, VALUE> *;
    auto RandomHeight() -> int;
    auto Equal(const KEY& a, const KEY &b) -> bool const;
    auto KeyIsAfterNode(const KEY& key, Node<KEY, VALUE> *n) -> bool const;
    auto FindGreaterOrEqual(const KEY& key, Node<KEY, VALUE> **prev) -> Node<KEY, VALUE> * const;
    auto FindLessThan(const KEY& key) -> Node<KEY, VALUE> * const;
    auto FindLast() -> Node<KEY, VALUE> * const;
};

template<typename KEY, typename VALUE, class Comparator>
SkipList<KEY, VALUE, Comparator>::SkipList(Comparator cmp, std::shared_ptr<Mem_Allocator> mem) {
    comparator = cmp;
    allocator = std::move(mem);
    head = NewNode(0, 0, maxHeight);
    max_height = 1;
    rand = Random(0xdeadbeaf);
    for (auto i = 0; i < maxHeight; i++) {
        head->SetNext(i, nullptr);
    }
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::MakeInstance(Comparator cmp, std::shared_ptr<Mem_Allocator> mem) -> std::shared_ptr<SkipList> const {
    return std::make_shared<SkipList>(cmp, mem);
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::Insert(const KEY &key, const VALUE &value) -> void {
    Node<KEY, VALUE> *prev[maxHeight];
    auto x = FindGreaterOrEqual(key, prev);
    assert(x == nullptr || !Equal(key, x->key));
    auto height = RandomHeight();
    if (height > getMaxHeight()) {
        for (auto i = getMaxHeight(); i < height; i++) {
            prev[i] = head;
        }
        max_height.store(height, std::memory_order_release);
    }
    x = NewNode(key, value, height);
    for (auto i = 0; i < height; i++) {
        x->SetNext(i, prev[i]->Next(i));
        prev[i]->SetNext(i, x);
    }
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::Contains(const KEY &key) -> bool {
    auto x = FindGreaterOrEqual(key, nullptr);
    if (x != nullptr && Equal(key, x->key)) {
        return true;
    } else {
        return false;
    }
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::getMaxHeight() -> int const {
    return max_height.load(std::memory_order_acquire);
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::NewNode(const KEY& k, const VALUE& v, int height) -> Node<KEY, VALUE> * {
    auto mem_ptr = allocator->Allocate(sizeof(Node<KEY, VALUE>) + sizeof(std::atomic<Node<KEY, VALUE> *>) * (height - 1));
    return new (mem_ptr) Node<KEY, VALUE>(k, v);
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::RandomHeight() -> int {
    static const uint32_t kb = 4;
    auto height(1);
    while (height < kMaxHeight && rnd_.OneIn(kBranching)) {
        height++;
    }
    assert(height > 0);
    assert(height <= kMaxHeight);
    return height;
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::Equal(const KEY& a, const KEY& b) -> bool const {
    return (comparator(a, b) == 0);
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::KeyIsAfterNode(const KEY& key, Node<KEY, VALUE> *n) -> bool const {
    return (n != nullptr) && (comparator(n->key, key) < 0);
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::FindGreaterOrEqual(const KEY& key, Node<KEY, VALUE> **prev) -> Node<KEY, VALUE> * const {
    auto x = head;
    auto level(getMaxHeight() - 1);
    while (true) {
        auto next = x->Next(level);
        if (KeyIsAfterNode(key, next)) {
            x = next;
        } else {
            if (prev != nullptr) {
                prev[level] = x;
            }
            if (level == 0) {
                return next;
            } else {
                level--;
            }
        }
    }
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::FindLessThan(const KEY& key) -> Node<KEY, VALUE> * const {
    auto x = head;
    auto level(getMaxHeight() - 1);
    while (true) {
        assert(x == head || comparator(x->key, key) < 0);
        auto next = x->Next(level);
        if (next == nullptr || comparator(next->key, key) >= 0) {
            if (level == 0) {
                return x;
            } else {
                level--;
            }
        } else {
            x = next;
        }
    }
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::FindLast() -> Node<KEY, VALUE> * const {
    auto x = head;
    auto level(getMaxHeight() - 1);
    while (true) {
        auto next = x->Next(level);
        if (next == nullptr) {
            if (level == 0) {
                return x;
            } else {
                level--;
            }
        } else {
            x = next;
        }
    }
}

template<typename KEY, typename VALUE, class Comparator>
class iterator
{
public:
    explicit iterator(std::shared_ptr<SkipList<KEY, VALUE, Comparator>> list);
    auto Valid() -> bool const;
    auto Key() -> KEY const;
    auto Value() -> VALUE const;
    auto Next() -> void;
    auto Prev() -> void;
    auto Seek(const KEY& target) -> void;
    auto SeekToFirst() -> void;
    auto SeekToLast() -> void;

private:
    std::shared_ptr<SkipList<KEY, VALUE, Comparator>> list;
    Node<KEY, VALUE> *node;
};


template<typename KEY, typename VALUE, class Comparator>
iterator<KEY, VALUE, Comparator>::iterator(std::shared_ptr<SkipList<KEY, VALUE, Comparator>> list) : list(list) {
    node = nullptr;
}

template<typename KEY, typename VALUE, class Comparator>
auto iterator<KEY, VALUE, Comparator>::Valid() -> bool const {
    return node != nullptr;
}

template<typename KEY, typename VALUE, class Comparator>
auto iterator<KEY, VALUE, Comparator>::Key() -> KEY const {
    assert(Valid());
    return node->key;
}

template<typename KEY, typename VALUE, class Comparator>
auto iterator<KEY, VALUE, Comparator>::Value() -> VALUE const {
    assert(Valid());
    return node->value;
}

template<typename KEY, typename VALUE, class Comparator>
auto iterator<KEY, VALUE, Comparator>::Next() -> void {
    assert(Valid());
    node = node->Next(0);
}

template<typename KEY, typename VALUE, class Comparator>
auto iterator<KEY, VALUE, Comparator>::Prev() -> void {
    assert(Valid());
    node = list->FindLessThan(node->key);
    if (node == list->head) {
        node = nullptr;
    }
}

template<typename KEY, typename VALUE, class Comparator>
auto iterator<KEY, VALUE, Comparator>::Seek(const KEY& target) -> void {
    node = list->FindGreaterOrEqual(target, nullptr);
}

template<typename KEY, typename VALUE, class Comparator>
auto iterator<KEY, VALUE, Comparator>::SeekToFirst() -> void {
    node = list->head->Next(0);
}

template<typename KEY, typename VALUE, class Comparator>
auto iterator<KEY, VALUE, Comparator>::SeekToLast() -> void {
    node = list->FindLast();
    if (node == list->head) {
        node = nullptr;
    }
}