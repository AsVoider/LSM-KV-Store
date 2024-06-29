#include <atomic>
#include <cstdlib>
#include <cassert>

#include "mem_pool.h"
#include "random.h"

template<typename KEY, typename VALUE, class Comparator>
class SkipList;

template<typename KEY, typename VALUE, class Comparator>
class Iterator;

template<typename KEY, typename VALUE>
struct Node
{
    explicit Node(const KEY k, const VALUE v) : key(k), value(v) {};

    KEY const key;
    VALUE const value;
    std::atomic<Node*> next[1];

    auto Next(int n) -> Node * {
        return next[n].load(std::memory_order_acquire);
    }

    auto SetNext(int n, Node* x) -> void {
        next[n].store(x, std::memory_order_release);
    }

    auto NoBarrier_Next(int n) -> Node * {
        assert(n >= 0);
        return next[n].load(std::memory_order_relaxed);
    }

    auto NoBarrier_SetNext(int n, Node* x) -> void {
        assert(n >= 0);
        next[n].store(x, std::memory_order_relaxed);
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
    
    auto Insert(const KEY key, const VALUE value) -> void;
    auto Contains(const KEY &key) -> bool;

private:
    int maxHeight = 4;
    Random rnd;
    Comparator const comparator;
    std::shared_ptr<Mem_Allocator> allocator;
    std::atomic<int> max_height;
    Node<KEY, VALUE> *const head;
    
    auto getMaxHeight() -> int const;
    auto NewNode(const KEY k, const VALUE v, int height) -> Node<KEY, VALUE> *;
    auto RandomHeight() -> int;
    auto Equal(const KEY& a, const KEY &b) -> bool const;
    auto KeyIsAfterNode(const KEY& key, Node<KEY, VALUE> *n) -> bool const;
    auto FindGreaterOrEqual(const KEY& key, Node<KEY, VALUE> **prev) -> Node<KEY, VALUE> * const;
    auto FindLessThan(const KEY& key) -> Node<KEY, VALUE> * const;
    auto FindLast() -> Node<KEY, VALUE> * const;
};

template<typename KEY, typename VALUE, class Comparator>
SkipList<KEY, VALUE, Comparator>::SkipList(Comparator cmp, std::shared_ptr<Mem_Allocator> mem) : 
    rnd(0xdeadbeef), 
    comparator(cmp),
    allocator(std::move(mem)),
    head(NewNode("", "", maxHeight)) {
    max_height = 1;
    // printf("mem ptr is %lx", allocator.get());
    for (auto i = 0; i < maxHeight; i++) {
        head->SetNext(i, nullptr);
    }
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::MakeInstance(Comparator cmp, std::shared_ptr<Mem_Allocator> mem) -> std::shared_ptr<SkipList> const {
    //printf("into make instance\n");
    auto ptr = std::make_shared<SkipList>(cmp, mem);
    // printf("mem ptr is %lx", ptr->allocator.get());
    return ptr;
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::Insert(const KEY key, const VALUE value) -> void {
    // if (head->Next(0)) {
    //     printf("At Insert key is %s addr is %lx\n", head->Next(0)->key, head->Next(0));
    // }
    Node<KEY, VALUE> *prev[maxHeight];
    auto x = FindGreaterOrEqual(key, prev);
    assert(x == nullptr || !Equal(key, x->key));
    auto height = RandomHeight();
    printf("max height is %d now height is %d\n", getMaxHeight(), height);
    if (height > getMaxHeight()) {
        for (auto i = getMaxHeight(); i < height; i++) {
            prev[i] = head;
        }
        max_height.store(height, std::memory_order_release);
    }
    x = NewNode(key, value, height);
    for (auto i = 0; i < height; i++) {
        x->SetNext(i, prev[i]/*->Next(i)*/);
        prev[i]->SetNext(i, x);
    }
    // for (auto i = 0; i < height; i++) {
    //     //printf("x ky is %s, x.next is %lx\n", x->key, x->Next(i));
    //     printf("head next is %lx, head next %d is %s, addr is %lx\n", static_cast<void *>(head->Next(i)), 
    //         i, head->Next(i)->key, head->Next(i)->key);
    // }
    printf("insert success\n");

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
auto SkipList<KEY, VALUE, Comparator>::NewNode(const KEY k, const VALUE v, int height) -> Node<KEY, VALUE> * {
    auto mem_ptr = allocator->Allocate(sizeof(Node<KEY, VALUE>) + sizeof(std::atomic<Node<KEY, VALUE> *>) * (height - 1));
    return new (mem_ptr) Node<KEY, VALUE>(k, v);
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::RandomHeight() -> int {
    static const uint32_t kb = 4;
    auto height(1);
    while (height < maxHeight && rnd.OneIn(kb)) {
        height++;
    }
    assert(height > 0);
    assert(height <= maxHeight);
    return height;
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::Equal(const KEY& a, const KEY& b) -> bool const {
    // printf("a is %s, b is %s\n", a, b);
    return comparator.compare(a, b) == 0;
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::KeyIsAfterNode(const KEY& key, Node<KEY, VALUE> *n) -> bool const {
    if (n)
        std::cout << "key is " << key << "n.key is " << n->key << std::endl;
    return (n != nullptr) && (comparator.compare(n->key, key) < 0);
}

template<typename KEY, typename VALUE, class Comparator>
auto SkipList<KEY, VALUE, Comparator>::FindGreaterOrEqual(const KEY& key, Node<KEY, VALUE> **prev) -> Node<KEY, VALUE> * const {
    auto x = head;
    auto level(getMaxHeight() - 1);
    //printf("head next key is %s\n", head->Next(level)->key);
    while (true) {
        auto next = x->Next(level);
        if (head->Next(level))
            printf("x . next key is %s, key addr is %lx, next is %d\n", next->key.c_str(), next->key.c_str(), level);
        if (KeyIsAfterNode(key, next)) {
            printf("after a node\n");
            x = next;
        } else {
            if (prev != nullptr) {
                printf("into here\n");
                prev[level] = x;
            }
            if (level == 0) {
                printf("return here\n");
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