#pragma once

#include <source_location>
#include <format>
#include <cstdio>

#include "utils.h"

namespace lain {

[[noreturn]] void term(const std::string& msg) {
    std::fputs(msg.c_str(), stderr);
    std::abort();
}

template <class... Args>
struct error_format {
    template <class T>
    consteval error_format(
        const T& s,
        std::source_location loc = std::source_location::current()) noexcept
        : fmt{s}, loc{loc} {}
    
    std::format_string<Args...> fmt;
    std::source_location loc;
};

template <class... Args>
void raise_error (
    const char* color,
    const char* prefix,
    error_format<std::type_identity_t<Args>...> fmt,
    Args&&... args) 
{
    auto loc = std::format("{}:{}", fmt.loc.file_name(), fmt.loc.line());
    auto msg = std::format(
        "{}[{}]{} {}{}{} {}\n",
        color, prefix, Ansi::Reset,
        Ansi::Bold, loc, Ansi::Reset,
        std::format(fmt.fmt, std::forward<Args>(args)...));
    term(msg);
}

template <class... Args>
void panic (error_format<std::type_identity_t<Args>...> fmt, Args&&... args) {
    raise_error(Ansi::RedFB, "panic", fmt, std::forward<Args>(args)...);
}

template <class... Args>
void todo (error_format<std::type_identity_t<Args>...> fmt, Args&&... args) {
    raise_error(Ansi::YellowFB, "todo", fmt, std::forward<Args>(args)...);
}

template <class... Args>
void unexpected(error_format<std::type_identity_t<Args>...> fmt, Args&&... args) {
    raise_error(Ansi::MagentaFB, "unexpected", fmt, std::forward<Args>(args)...);
}

}