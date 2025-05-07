#pragma once

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <array>
#include <span>
#include <new>

namespace lain {

using uint = unsigned int;

struct Ansi {
  static constexpr auto MagentaFB = "\033[35;1m"; // Magenta Foreground Bold
  static constexpr auto YellowFB  = "\033[33;1m"; // Yellow Foreground Bold
  static constexpr auto BlueFB    = "\033[34;1m"; // Blue Foreground Bold
  static constexpr auto RedFB     = "\033[31;1m"; // Red Foreground Bold
  static constexpr auto Bold      = "\033[1m";
  static constexpr auto Reset     = "\033[0m"; 
};

template<typename T, std::size_t N>
constexpr auto array_make(const T (&arr)[N]) {
    std::array<T, N> ret;
    for (std::size_t i = 0; i < N; ++i) {
        ret[i] = arr[i];
    }
    return ret;
}

std::unique_ptr<char[]> file_read(const std::string &path, std::size_t& size) noexcept {
    auto in = std::ifstream(path, std::ios::in | std::ios::ate);
    if (!in.is_open()) {
        return nullptr;
    }

    auto end = in.tellg();
    if (end < 0) {
        return nullptr;
    }
    const auto length = static_cast<std::size_t>(end);

    in.seekg(0, std::ios::beg);

    auto buffer = std::unique_ptr<char[]>(new (std::nothrow) char[length + 1]);
    if (!buffer) {
        return nullptr;
    }

    in.read(buffer.get(), static_cast<std::streamsize>(length));
    if (!in && !in.eof()) {
        return nullptr;
    }

    const auto read = in.gcount();
    buffer[read] = '\0';
    size = static_cast<std::size_t>(read);

    return buffer;
}


}