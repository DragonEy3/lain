#pragma once

#include <sstream>
#include <cassert>
#include <cstdio>
#include <memory>
#include <vector>

#include "token.h"
#include "error.h"

namespace lain {

struct Location {
    std::string_view file;
    std::string_view line;
    uint row, col;
};

class Lexer {
    std::string path;
    std::unique_ptr<char[]> src;
    std::size_t len;

    std::vector<std::string_view> rows;
    std::vector<Token> tokens;

    uint crs = 0, col = 0, row = 0;

    bool eof () const;
    char peek() const;
    char get();

    bool skipComment();
    bool skipSpace();

    void scanIdentifier(Token &token);
    void scanNumeric(Token &token);
    void scanString(Token &token);
    void scanSymbol(Token &token);
    void scanToken();

    template<typename... Args>
    void lexical_error(const std::format_string<Args...> fmt, Args&&... args);
    
public:
    explicit Lexer (const std::string &path);

    const std::vector<Token> &scan ();

    const std::string &name () const;

    const std::string_view line (std::size_t num) const;
};

Lexer::Lexer (const std::string &path) : path(path) {
    src = file_read(path, len);
    if (!src) {
        panic("could not read source file {}", path);
    }
    if (len > UINT32_MAX) {
        panic("source file {} too large", path);
    }
}

const std::string &Lexer::name () const {
    return path;
}

const std::string_view Lexer::line (std::size_t num) const {
    if (num >= rows.size()) {
        panic("Invalid line access {} in {}", num, path);
    }
    return rows[num];
}


bool Lexer::eof() const {
    return crs >= len;
}

char Lexer::peek () const {
    if (eof()) {
        return 0;
    }
    return src[crs];
}

char Lexer::get () {
    if (eof()) {
        return 0;
    }
    char c = src[crs];
    if (c == '\n') {
        rows.emplace_back(src.get() + crs - col, col);
        col = 0;
        row++;
    } else {
        col++;
    }
    crs++;
    return c;
}

bool Lexer::skipComment() {
    if (peek() != '#') {
        return false;
    }

    while (!eof() && get() != '\n');
    return true;
}

bool Lexer::skipSpace() {
    if (!isspace(peek())) {
        return false;
    }
    while (isspace(peek())) {
        get();
    }
    return true;
}

void Lexer::scanNumeric(Token &token) {
    std::stringstream ss;
    auto pos = crs;

    while (isdigit(peek())) {
        ss << get();
    }

    token.len = crs - pos;
    token.num = std::stoul(ss.str());
    token.type = Token::Integer;
}

void Lexer::scanIdentifier(Token &token) {
    std::stringstream ss;
    auto pos = crs;

    while (isalnum(peek())) {
        ss << get();
    }

    token.len = crs - pos;
    token.str = ss.str();

    auto type = check_type(token.str);
    if (type == Token::Unknown) {
        token.type = Token::Identifier;
    } else {
        token.type = type;
    }
}

void Lexer::scanString(Token &token) {
    std::stringstream ss;
    auto pos = crs;
    get();

    while (!eof() && peek() != '"') {
        if (peek() == '\n') {
            lexical_error("newline in string literal.");
        } else if (peek() == '\\') {
            todo("string escapes");
        }
        ss << get();
    }
    get();

    if (eof()) {
        lexical_error("incomplete string literal");
    }

    token.len = crs - pos;
    token.str = ss.str();
    token.type = Token::String;
}

void Lexer::scanSymbol(Token &token) {
    auto res = tokenize(src.get() + crs);

    if (res.second.first == Token::Unknown) {
        lexical_error("unexpected character {}", peek());
    }

    token.len = res.first.size();
    token.type = res.second.first;

    crs += token.len;
    col += token.len;
}

void Lexer::scanToken() {
    Token &token = tokens.emplace_back();

    token.row = row;
    token.col = col;

    char c = peek();

    if (isalpha(c)) {
        scanIdentifier(token);
    } else if (isdigit(c)) {
        scanNumeric(token);
    } else if (c == '"') {
        scanString(token);
    } else {
        scanSymbol(token);
    }
}

const std::vector<Token> &Lexer::scan () {
    while (!eof()) {
        if (skipComment()) {
            continue;
        }
        if (skipSpace()) {
            continue;
        }
        scanToken();
    }
    Token &eof = tokens.emplace_back();
    eof.type = Token::Type::Eof;
    return tokens;
}

template<typename... Args>
void Lexer::lexical_error(const std::format_string<Args...> fmt, Args&&... args) {
    std::string_view line;
    auto str = src.get() + crs - col;
    if (src[crs] == '\n') {
        line = {str, col};
    } else {
        line = {str, col + 1};
    }

    std::string error_msg = std::format(fmt, std::forward<Args>(args)...);
    
    std::string msg = std::format(
        "{}{}:{}:{}:{} {}error:{} {}\n{}\n{:>{}}{}^{}\n", 
        Ansi::Bold, path, row + 1, col + 1, Ansi::Reset,
        Ansi::RedFB, Ansi::Reset, error_msg,
        line, "", line.size() - 1, Ansi::RedFB, Ansi::Reset
    );
    
    term(msg);
}

}