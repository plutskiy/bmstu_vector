#pragma once

#include "raw_memory.h"
#include <algorithm>
#include <type_traits>
#include <iostream>


namespace bmstu {
    template<typename T>
    class vector {
    public:

        struct iterator {
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;

            iterator(pointer ptr) : m_ptr(ptr) {}


            reference operator*() const {
                return *m_ptr;
            }

            pointer operator->() {
                return m_ptr;
            }

            iterator &operator++() {
                ++m_ptr;
                return *this;
            }

            iterator &operator--() {
                --m_ptr;
                return *this;
            }

            iterator &operator=(const iterator &other) {
                this->m_ptr = other.m_ptr;
                return *this;
            }

            iterator operator++(int) {
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            iterator operator--(int) {
                iterator tmp = *this;
                --(*this);
                return tmp;
            }

            friend bool operator==(const iterator &a, const iterator &b) {
                return a.m_ptr == b.m_ptr;
            }

            friend bool operator!=(const iterator &a, const iterator &b) {
                return !(a == b);
            }

            friend ptrdiff_t operator-(const iterator &a, const iterator &b) {
                return a.m_ptr - b.m_ptr;
            }

            iterator &operator+(size_t n) noexcept {
                m_ptr = m_ptr + n;
                return *this;
            }

            iterator &operator-(size_t n) noexcept {
                m_ptr = m_ptr - n;
                return *this;
            }

        private:
            pointer m_ptr;
        };

        using const_iterator = const iterator;

        vector() = default;


        explicit vector(size_t size) : data_(size), size_(size) {
            if constexpr (std::is_default_constructible_v<T>) {
                std::uninitialized_value_construct_n(data_.get_address(), size);
            } else {
                uint8_t *ptr = reinterpret_cast<uint8_t *>(static_cast<void *>(data_.get_address()));
                std::fill(ptr, ptr + (size * sizeof(T)), 0);
            }
        }


        vector(std::initializer_list<T> ilist) : data_(ilist.size()), size_(ilist.size()) {
            std::copy(ilist.begin(), ilist.end(), begin());
        }


        vector(const vector &other) : data_(other.size_), size_(other.size_) {
            std::uninitialized_copy_n(other.data_.get_address(), other.size_, data_.get_address());
        }

        vector(vector &&other) noexcept {
            std::swap(data_, other.data_);
            std::swap(size_, other.size_);
        }

        vector &operator=(const vector &other) noexcept {
            if (this != other) {
                if (other.size_ > data_.capacity()) {
                    vector copy(other);
                    swap(copy);
                } else {
                    if (!(size_ < other.size_)) {
                        std::copy_n(other.data_.get_address(), other.size_, data_.get_address());
                        std::destroy_n(data_.get_address() + other.size_, size_ - other.size_);
                        size_ = other.size_;
                    } else {
                        std::copy_n(other.data_.get_address(), other.size_, data_.get_address());
                        std::uninitialized_copy_n(other.data_.get_address() + size_, other.size_ - size_,
                                                  data_.get_address() + size_);
                        size_ = other.size_;
                    }
                }
            }
            return *this;
        }

        vector &operator=(vector &&right) {
            if (this != &right) {
                data_ = std::move(right.data_);
                size_ = right.size_;
                right.size_ = 0;
            }
            return *this;
        }

        ~vector() {
            if (size_ != 0) {
                std::destroy_n(data_.get_address(), size_);
            }
        }

        iterator begin() {
            return data_.get_address();
        }

        iterator end() {
            return data_.get_address() + size_;
        }

        const_iterator begin() const {
            return data_.get_address();
        }

        const_iterator end() const {
            return data_.get_address() + size_;
        }

        const_iterator cbegin() const {
            return data_.get_address();
        }

        const_iterator cend() const {
            return data_.get_address() + size_;
        }

        typename iterator::reference operator[](size_t index) noexcept {
            return data_[index];
        }

        typename const_iterator::reference operator[](size_t index) const noexcept {
            return const_cast <typename const_iterator::reference> (data_[index]);
        }

        typename iterator::reference at(size_t index) {
            if (index > size_) {
                throw std::out_of_range("Invalid index");
            } else {
                return data_[index];
            }
        }

        typename const_iterator::reference at(size_t index) const {
            if (index > size_) {
                throw std::out_of_range("Invalid index");
            } else {
                return const_cast <typename const_iterator::value_type>(data_[index]);
            }

        }


        void clear() noexcept {
            size_ = 0;
        }

        friend void swap(vector<T> &left, vector<T> &right) {
            std::swap(left.data_, right.data_);
            std::swap(left.size_, right.size_);
        }

        void reserve(size_t new_capaity) {
            if (new_capaity <= data_.capacity()) {
                return;
            }
            raw_memory<T> new_data(new_capaity);
            if constexpr (std::is_move_constructible_v<T>) {
                std::uninitialized_move_n(data_.get_address(), size_, new_data.get_address());
            } else {
                std::uninitialized_copy_n(data_.get_address(), size_, new_data.get_address());
            }
            std::destroy_n(data_.get_address(), size_);
            data_.swap(new_data);
        }

        void resize(size_t new_size) {
            if (new_size < size_) {
                std::destroy_n(data_.get_address() + new_size, size_ - new_size);
                size_ = new_size;
            } else {
                if (new_size > capacity()) {
                    reserve(new_size * 2);
                }
                raw_memory<T> new_data(new_size);
                std::uninitialized_copy_n(data_.get_address(), size_, new_data.get_address());
                data_.swap(new_data);
                size_ = new_size;
            }
        }

        void pop_back() noexcept {
            std::destroy_at(data_.get_address() + size_);
            --size_;
        }

        template<typename ... Args>
        T &emplace_back(Args &&... args) {
            if (size_ == capacity()) {
                auto new_capacity = (size_ == 0) ? 1 : size_ * 2;
                raw_memory<T> new_data(new_capacity);
                new(new_data.get_address() + size_) T(std::forward<Args>(args) ...);
                if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                    std::uninitialized_move_n(data_.get_address(), size_, new_data.get_address());
                } else {
                    std::uninitialized_copy_n(data_.get_address(), size_, new_data.get_address());
                }
                std::destroy_n(data_.get_address(), size_);
                data_.swap(new_data);
            } else {
                new(data_.get_address() + size_) T(std::forward<Args>(args) ...);
            }
            ++size_;
            return data_[size_ - 1];
        }

        template<typename ... Args>
        iterator emplace(const_iterator pos, Args &&... args) {
            iterator res_pos = begin();
            if (pos == cend()) {
                res_pos = &emplace_back(std::forward<Args>(args) ...);
            } else if (size_ == data_.capacity()) {
                auto new_capacity = (size_ == 0) ? 1 : size_ * 2;
                raw_memory<T> new_data(new_capacity);
                const size_t dest_pos = pos - begin();
                if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                    std::uninitialized_move_n(data_.get_address(), dest_pos, new_data.get_address());
                } else {
                    try {
                        std::uninitialized_copy_n(data_.get_address(), dest_pos, new_data.get_address());
                    } catch (...) {
                        std::destroy_n(new_data + dest_pos, 1);
                        throw;
                    }
                }
                if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_nothrow_copy_constructible_v<T>) {
                    std::uninitialized_move_n(data_.get_address() + dest_pos, size_ - dest_pos,
                                              new_data.get_address() + dest_pos + 1);
                } else {
                    try {
                        std::uninitialized_copy_n(data_.get_address() + dest_pos, size_ - dest_pos,
                                                  new_data.get_address() + dest_pos + 1);
                    } catch (...) {
                        std::destroy_n(new_data.get_address(), dest_pos + 1);
                        throw;
                    }
                }
                if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                    new_data[dest_pos] = std::move(args...);
                } else {
                    std::copy(new_data[dest_pos], args...);
                }
                std::destroy_n(data_.get_address(), size_);
                data_.swap(new_data);
                res_pos = begin() + dest_pos;
                ++size_;
                return res_pos;
            } else {
                T tmp(std::forward<Args>(args) ...);
                new(data_.get_address() + size_) T(std::move(data_[size_ - 1]));
                res_pos = begin() + (pos - begin());
                std::move_backward(res_pos, end() - 1, end());
                *res_pos = std::move(tmp);
                ++size_;
                return res_pos;
            }
        }

        iterator erase(const_iterator pos) {
            iterator res_it = begin() + (pos - begin());
            std::move(res_it + 1, end(), res_it);
            std::destroy_n(end() - 1, 1);
            --size_;
            return res_it;

        }

        template<typename Type>
        iterator incert(const_iterator pos, Type &&value) {
            return emplace(pos, std::forward<Type>(value));
        }

        template<typename Type>
        void push_back(Type &&value) {
            emplace_back(std::forward<Type>(value));
        }

        size_t size() {
            return size_;
        }

        size_t capacity() {
            return data_.capacity();
        }

        bool empty() const noexcept {
            return (size_ == 0);
        }

        friend bool operator==(bmstu::vector<T> &l, bmstu::vector<T> &r) {
            if (l.size() == r.size()) {
                for (size_t i = 0; i < l.size(); ++i) {
                    if (l[i] != r[i]) {
                        return false;
                    }
                }
                return true;
            }
            return false;
        }

        friend bool operator!=(const vector<T> &l, const vector<T> &r) {
            return !(l == r);
        }

        friend bool operator<(const vector<T> &l, const vector<T> &r) {
            return lexicographical_compare_(l, r);
        }

        friend bool operator>(const vector<T> &l, const vector<T> &r) {
            return (r < l);
        }

        friend bool operator<=(const vector<T> &l, const vector<T> &r) {
            return !(r < l);
        }

        friend bool operator>=(const vector<T> &l, const vector<T> &r) {
            return !(l < r);
        }

        template<class S>
        friend S &operator<<(S &os, const vector<T> &other) {
            os << "[" << other[0];
            for(size_t i = 1; i != other.size_; ++i ){
                os << ", " << other[i];
            }
            os << "]";

            return os;
        }

    private:
        static bool lexicographical_compare_(const vector<T> &l, const vector<T> &r) {
            auto lf = l.begin(), rf = r.begin();
            for (; (lf != l.end()) && (rf != r.end()); ++lf, ++rf) {
                if (*lf < *rf) {
                    return true;
                }
                if (*rf < *lf) {
                    return false;
                }
            }
            return (rf != r.end()) && (lf == l.end());
        }

        void swap_(vector<T> other) {
            std::swap(*this, other);
        }

        raw_memory<T> data_;
        size_t size_ = 0;
    };
}