#ifndef MSKIPLIST_HEADER
#define MSKIPLIST_HEADER

#include <atomic>
#include <cassert>
#include <random>
#include <cstring>

template <typename T, typename P>
class SkipList {
public:
    // 插入数据
    void insert(const T& key, const P& value);
    // 删除数据（若没有找到节点返回失败）
    bool remove(const T& key);
    // 查询数据
    bool search(const T& key, P& value);
    // 是否包含某个数据
    bool contains(const T& key);
    // 当前表节点个数
    std::size_t count() const;
    // 清除表
    void clear();

public:
    SkipList(int max_level = 12);
    ~SkipList();

private:
    struct SkipNode;
    std::size_t          count_{};
    SkipNode**           pre_{};
    SkipNode*            header_{};
    const int            max_level_{};       // 限定的最大高度
    std::atomic_int      cur_max_height_{};  // 当前使用的最大高度
    std::random_device   rd_{};
    std::mt19937         gen_{};
    std::uniform_int_distribution<int> dis_{};

private:
    int       random_level();
    SkipNode* find_node(const T& key);
};

template <typename T, typename P>
SkipList<T, P>::~SkipList()
{
    clear();
    delete[] pre_;
    delete header_;
}

template <typename T, typename P>
void SkipList<T, P>::clear()
{
    SkipNode* start = header_->get_no_bar(0);
    while (start) {
        SkipNode* n = start;
        start = start->get_no_bar(0);
        delete n;
        --count_;
    }
}

template <typename T, typename P>
inline int SkipList<T, P>::random_level()
{
    static const int base_ = 2;
    int              height = 1;
    while (height < max_level_ && (dis_(gen_) % base_) == 0) {
        ++height;
    }
    return height;
}

template <typename T, typename P>
struct SkipList<T, P>::SkipNode {
    T   key_{};
    P   value_{};
    int level_{};

public:
    explicit SkipNode(int level = 1) { alloc(level); }
    ~SkipNode() { release(); }
    SkipNode(const T& key, const P& value, int level)
    {
        key_ = key;
        value_ = value;
        alloc(level);
    }
    void alloc(int max_level)
    {
        if (max_level < 1) {
            return;
        }
        release();
        next_ = new std::atomic<SkipNode*>[max_level] {};
    }
    void      release() { delete[] next_; }
    SkipNode* get(int n)
    {
        assert(n >= 0);
        return next_[n].load(std::memory_order_acquire);
    }
    SkipNode* get_no_bar(int n)
    {
        assert(n >= 0);
        return next_[n].load(std::memory_order_relaxed);
    }
    void set(int n, SkipNode* node)
    {
        assert(n >= 0);
        next_[n].store(node, std::memory_order_release);
    }
    void set_no_bar(int n, SkipNode* node)
    {
        assert(n >= 0);
        next_[n].store(node, std::memory_order_relaxed);
    }

private:
    std::atomic<SkipNode*>* next_{};
};

template <typename T, typename P>
inline SkipList<T, P>::SkipList(int max_level)
    : max_level_(max_level),
      cur_max_height_(1),
      gen_(rd_()),
      dis_(0, std::numeric_limits<int>::max())
{
    assert(max_level_ > 0);
    header_ = new SkipNode(max_level_);
    pre_ = new SkipNode*[max_level_];
}

template <typename T, typename P>
typename SkipList<T, P>::SkipNode* SkipList<T, P>::find_node(const T& key)
{
    memset(pre_, 0x0, sizeof(SkipNode*) * max_level_);
    SkipNode* x = header_;
    int       level = cur_max_height_.load() - 1;
    while (true) {
        SkipNode* next = x->get(level);
        if (next && next->key_ < key) {
            x = next;
        } else {
            pre_[level] = x;
            if (level == 0) {
                return next;
            } else {
                --level;
            }
        }
    }
}

template <typename T, typename P>
inline bool SkipList<T, P>::contains(const T& key)
{
    SkipNode* x = find_node(key);
    if (x && x->key_ == key) {
        return true;
    }
    return false;
}

template <typename T, typename P>
inline std::size_t SkipList<T, P>::count() const
{
    return count_;
}

template <typename T, typename P>
inline void SkipList<T, P>::insert(const T& key, const P& value)
{
    SkipNode* x = find_node(key);

    if (x && x->key_ == key) {
        x->value_ = value;
        return;
    }

    int height = random_level();
    if (height > cur_max_height_) {
        for (int i = cur_max_height_; i < height; ++i) {
            pre_[i] = header_;
        }
        cur_max_height_.store(height, std::memory_order_relaxed);
    }
    x = new SkipNode(key, value, height);
    for (int i = 0; i < height; ++i) {
        x->set_no_bar(i, pre_[i]->get_no_bar(i));
        pre_[i]->set(i, x);
    }
    ++count_;
}

template <typename T, typename P>
inline bool SkipList<T, P>::remove(const T& key)
{
    memset(pre_, 0x0, sizeof(SkipNode*) * max_level_);
    SkipNode* x = header_;
    SkipNode* purpose = nullptr;
    int       level = cur_max_height_.load() - 1;
    while (true) {
        if (level < 0) {
            break;
        }
        SkipNode* next = x->get(level);
        if (!next) {
            --level;
            continue;
        }

        if (next->key_ < key) {
            x = next;
            continue;
        }

        if (next->key_ == key) {
            SkipNode* nx = next->get_no_bar(level);
            x->set_no_bar(level, nx);
            purpose = next;
        }
        --level;
    }
    delete purpose;
    --count_;
    return true;
}

template <typename T, typename P>
inline bool SkipList<T, P>::search(const T& key, P& value)
{
    SkipNode* x = find_node(key);
    if (x && x->key_ == key) {
        value = x->value_;
        return true;
    }
    return false;
}

#endif
