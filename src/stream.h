#pragma once

#include "lexer.h"
#include "error.h"

namespace lain {

class TokenStream {
    Lexer lexer;

    const std::vector<Token> &tokens;
    std::size_t it = 0;

public:
    TokenStream (const std::string &path) 
        : lexer(path), tokens(lexer.scan()) {}

    const Token &peek (long off = 0) const {
        if (off < 0 && static_cast<std::size_t>(-off) > it) {
            syntax_error("Token expected before BOF");
        } else if (it + off > tokens.size()) {
            return tokens.back();
        }
        return tokens[it];
    }

    const Token &bump () {
        if (it >= tokens.size() - 1) {
            syntax_error("EOF bumped");
        }
        return tokens[it++];
    }

    const Token &consume (Token::Type type) {
        auto &token = tokens[it];
        if (token.type != type) {
            syntax_error("Expected {} not {}", to_string(type), to_string(token.type));
        }
        return bump();
    }

    bool done () const {
        return it >= tokens.size() - 1;
    }

    template<typename... Args>
    void syntax_error(const std::format_string<Args...> fmt, Args&&... args) const {
        const auto &token = tokens[it];
        
        auto loc = error_location(lexer.name(), token.row, token.col);

        auto line = lexer.line(token.row);
        auto pre = line.substr(0, token.col);
        auto tok = line.substr(token.col, token.len);
        auto post = line.substr(token.col + token.len);
        
        std::string error_msg = std::format(fmt, std::forward<Args>(args)...);
        
        std::string msg = std::format(
            "{} {}error:{} {}\n{}{}{}{}{}\n{:>{}}{}^{}\n", 
            loc, Ansi::RedFB, Ansi::Reset, error_msg,
            pre, Ansi::RedFB, tok, Ansi::Reset, post,
            "", pre.size(), Ansi::RedFB, Ansi::Reset
        );

        term(msg);
    }
};

}