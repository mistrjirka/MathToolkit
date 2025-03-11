#pragma once

#include <array>

template<typename T, size_t S>
class CircularBufferMT {
public:
    CircularBufferMT() : head_(0), tail_(0), full_(false) {}

    void put(T item) {
        buffer_[tail_] = item;
        
        if (full_) {
            head_ = (head_ + 1) % S;
        }
        
        tail_ = (tail_ + 1) % S;
        full_ = tail_ == head_;
    }

    const std::array<T, S>& get_buffer() const {
        return buffer_;
    }

    size_t get_head() const { return head_; }
    size_t get_tail() const { return tail_; }
    
    bool empty() const {
        return (!full_ && (head_ == tail_));
    }

    bool full() const {
        return full_;
    }

    size_t size() const {
        if (full_) return S;
        if (tail_ >= head_) return tail_ - head_;
        return S - (head_ - tail_);
    }

    // Iterator access for reading in correct order
    template<typename Func>
    void for_each(Func f) const {
        if (empty()) return;
        
        size_t idx = head_;
        size_t count = size();
        
        for (size_t i = 0; i < count; ++i) {
            f(buffer_[idx]);
            idx = (idx + 1) % S;
        }
    }

private:
    std::array<T, S> buffer_;
    size_t head_;
    size_t tail_;
    bool full_;
};
