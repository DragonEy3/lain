#pragma once

#include <string_view>
#include <cstdint>
#include <array>

template<class T, std::size_t N, class M>
consteval auto extract_key (const std::array<T,N>& arr, M T::* m) {
    std::array<M, N> result{};
    for (std::size_t i = 0; i < N; ++i) {
        result[i] = arr[i].*m;
    }
    return result;
}

template<class T, std::size_t N, class M1, class M2>
consteval auto extract_pair (const std::array<T,N>& arr, M1 T::* m1, M2 T::* m2) {
    std::array<std::pair<M1,M2>, N> result{};
    for (std::size_t i = 0; i < N; ++i) {
        result[i] = { arr[i].*m1, arr[i].*m2 };
    }
    return result;
}

template <std::size_t N>
consteval auto max_len (const std::array<std::string_view, N>& arr) {
    std::size_t len = 0;
    for (const std::string_view &val: arr) {
        len = std::max(len, val.size());
    }
    return len;
}