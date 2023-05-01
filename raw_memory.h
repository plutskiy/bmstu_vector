#pragma once

#include <memory>
#include <cassert>

namespace bmstu {
    template<typename T>
    class raw_memory {
    public:
        raw_memory() = default;

        explicit raw_memory(size_t cap) : buffer_(allocate_(cap)), capacity_(cap) {}

        raw_memory(const raw_memory &other) = delete;

        raw_memory &operator=(const raw_memory &other) = delete;

        raw_memory &operator=(raw_memory &&other) noexcept {
            if (buffer_) {
                deallocate_(buffer_);
            }
            buffer_ = std::exchange(other.buffer_, nullptr);
            capacity_ = std::exchange(other.capacity_, 0);
            return *this;
        }

        raw_memory(raw_memory &&other) : buffer_(std::exchange(other.buffer_, nullptr)),
                                         capacity_(std::exchange(other.capacity_, 0)) {}

        T *operator+(size_t offset) noexcept {
            assert(offset <= capacity_);
            return buffer_ + offset;
        }

        const T *operator+(size_t offset) const noexcept {
            assert(offset <= capacity_);
            return const_cast<raw_memory &>(*this).buffer_ + offset;
        }

        T &operator[](size_t index) {
            assert(index < capacity_);
            return buffer_[index];
        }

        size_t capacity() const{
            return capacity_;
        }

        T * get_address() const {
            return buffer_;
        }

        const T &operator[](size_t index) const noexcept {
            return const_cast<raw_memory &> (*this)[index];
        }

        void swap(raw_memory &other) {
            std::swap(capacity_, other.capacity_);
            std::swap(buffer_, other.buffer_);
        }

        ~raw_memory() {
            deallocate_(buffer_);
        }

    private:
        T *allocate_(size_t n) {
            return n != 0 ? static_cast<T *>(operator new(n * sizeof(T))) : nullptr;
        }

        void deallocate_(T *buffer) {
            operator delete(buffer);
        }

        size_t capacity_ = 0;
        T *buffer_ = nullptr;
    };
}