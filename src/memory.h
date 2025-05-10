#pragma once

#include <type_traits>
#include <span>

#include "error.h"

namespace lain {

template <typename T>
class cptr {
    static_assert(
        std::is_trivially_destructible_v<T> &&
        std::is_trivially_copyable_v<T> &&
        std::is_standard_layout_v<T> &&
        alignof(T) <= alignof(std::max_align_t),
        "Type must be malloc-free compatible.");

    size_t cnt;
    T* raw;

public:
    cptr(const cptr&) = delete;
    cptr& operator=(const cptr&) = delete;
    cptr(cptr&&) = delete;
    cptr& operator=(cptr&&) = delete;

    T* begin() noexcept             { return raw; }
    const T* begin() const noexcept { return raw; }
    T* end() noexcept               { return raw + cnt; }
    const T* end() const noexcept   { return raw + cnt; }

    T* operator->() noexcept { return raw; }
    const T* operator->() const noexcept { return raw; }

    T& operator[](size_t i) noexcept { return raw[i]; }
    const T& operator[](size_t i) const noexcept { return raw[i]; }

    T& at(size_t i) {
        if (i >= cnt) panic("Bounds violation");
        return raw[i];
    }

    const T& at(size_t i) const {
        if (i >= cnt) panic("Bounds violation");
        return raw[i];
    }

    T* get() noexcept { return raw; }
    const T* get() const noexcept { return raw; }

    std::span<T> span() noexcept { return {raw, cnt}; }
    std::span<const T> span() const noexcept { return {raw, cnt}; }

    ~cptr() {
        free(raw);
    }

private:
    explicit cptr(size_t size) : cnt(size) {
        raw = static_cast<T*>(malloc(sizeof(T) * size));
        if (!raw) {
            panic("Allocation failure");
        }
    }

    friend cptr<T> make_cptr<T>(size_t size);
};

template <typename T>
[[nodiscard]] inline cptr<T> make_cptr(size_t size) {
    return cptr<T>(size);
}

template <typename T, size_t N>
struct Stack {
    static_assert(std::is_integral_v<T> || std::is_pointer_v<T> || std::is_enum_v<T>,
        "Stack only supports integer or pointer types");

    size_t len = 0;
    T data[N];

    T pop () {
        if (len == 0) {
            panic("Stack underflow");
        }
        return data[--len];
    }
    
    void push (const T t) {
        if (len >= N) {
            panic("Stack overflow");
        }
        data[len++] = t;
    }
};

template <typename T, typename... Args>
[[nodiscard]] inline T* alloc(Args&&... args) noexcept {
    T* ptr = nullptr;
    if constexpr (std::is_aggregate_v<T>) {
        ptr = new (std::nothrow) T{std::forward<Args>(args)...}; // brace-init
    } else {
        ptr = new (std::nothrow) T(std::forward<Args>(args)...); // paren-init
    }
    if (!ptr) {
        panic("Failed alloc");
    }
    return ptr;
}

}