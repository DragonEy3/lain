#pragma once

#include <unordered_map>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <span>

#include "utils.h"
#include "table.h"

namespace lain {

struct Token {
    enum Type {
        Unknown,

        Identifier, Eof,

        // Keywords
        Var,        Fun,        Return,     For,
        Continue,   Break,      If,         Else,
        Import,     Module,     Protected,  Private,
        Static,     Const,      Comp,       Unsafe,
        Unique,     Debug,      Struct,     Enum,

        // Literals
        String,         Character,
        Float,          Ipv4,
        Integer,        Color,

        // Types
        U8,     U16,    U32,    U64,    UInt,
        I8,     I16,    I32,    I64,    Int,
        Void,   F32,    F64,
        
        // Separators
        LParen,     RParen,     LBrace,     RBrace,
        LBracket,   RBracket,   Colon,      Semi,

        // Operators
        Assign,     Add,        Sub,        Mul,        Div,
        Increment,  AddEq,      SubEq,      MulEq,      DivEq,
        Equals,     Lesser,     Greater,    LessEq,     GreatEq,
        VBar,       Caret,      Ampersand,  Tilde,      Dot,
        LogOr,      LogAnd,     LogNot,     Comma,

    } type = Unknown;

    std::size_t row;
    std::size_t col;
    std::size_t len;
    std::string str;
    std::size_t num;
};

namespace Category {
    enum Category : uint32_t {
        None            = 0,
        Keyword         = 1 << 0,
        Separator       = 1 << 1,
        Operator        = 1 << 2,
        Literal         = 1 << 3,
        Modifier        = 1 << 4,
        Type            = 1 << 5,
        Tokenizable     = 1 << 6,
        Operand         = 1 << 7,
    };
}

struct TokenInfo {
    std::string_view str;
    Token::Type type;
    uint32_t categories;
};

constexpr auto reserved = array_make<TokenInfo>({
    {"unknown",     Token::Unknown,    Category::None},

    {"identifier",  Token::Identifier, Category::Operand},
    {"eof",         Token::Eof,        Category::None},

    // Keywords
    {"var",         Token::Var,        Category::Tokenizable | Category::Keyword},
    {"fun",         Token::Fun,        Category::Tokenizable | Category::Keyword},
    {"enum",        Token::Enum,       Category::Tokenizable | Category::Keyword},
    {"struct",      Token::Struct,     Category::Tokenizable | Category::Keyword},
    {"return",      Token::Return,     Category::Tokenizable | Category::Keyword},
    {"comp",        Token::Comp,       Category::Tokenizable | Category::Keyword},
    {"for",         Token::For,        Category::Tokenizable | Category::Keyword},
    {"continue",    Token::Continue,   Category::Tokenizable | Category::Keyword},
    {"break",       Token::Break,      Category::Tokenizable | Category::Keyword},
    {"if",          Token::If,         Category::Tokenizable | Category::Keyword},
    {"else",        Token::Else,       Category::Tokenizable | Category::Keyword},
    {"import",      Token::Import,     Category::Tokenizable | Category::Keyword},
    {"module",      Token::Module,     Category::Tokenizable | Category::Keyword},
    {"protected",   Token::Protected,  Category::Tokenizable | Category::Keyword | Category::Modifier},
    {"private",     Token::Private,    Category::Tokenizable | Category::Keyword | Category::Modifier},
    {"static",      Token::Static,     Category::Tokenizable | Category::Keyword | Category::Modifier},
    {"const",       Token::Const,      Category::Tokenizable | Category::Keyword | Category::Modifier},
    {"unique",      Token::Unique,     Category::Tokenizable | Category::Keyword | Category::Modifier},
    {"unsafe",      Token::Unsafe,     Category::Tokenizable | Category::Keyword | Category::Modifier},
    {"debug",       Token::Debug,      Category::Tokenizable | Category::Keyword | Category::Modifier},

    // Types
    {"u8",   Token::U8,   Category::Tokenizable | Category::Type},
    {"u16",  Token::U16,  Category::Tokenizable | Category::Type},
    {"u32",  Token::U32,  Category::Tokenizable | Category::Type},
    {"u64",  Token::U64,  Category::Tokenizable | Category::Type},
    {"uint", Token::UInt, Category::Tokenizable | Category::Type},
    {"i8",   Token::I8,   Category::Tokenizable | Category::Type},
    {"i16",  Token::I16,  Category::Tokenizable | Category::Type},
    {"i32",  Token::I32,  Category::Tokenizable | Category::Type},
    {"i64",  Token::I64,  Category::Tokenizable | Category::Type},
    {"int",  Token::Int,  Category::Tokenizable | Category::Type},
    {"void", Token::Void, Category::Tokenizable | Category::Type},

    // Separators
    {"(",    Token::LParen,    Category::Tokenizable | Category::Separator},
    {")",    Token::RParen,    Category::Tokenizable | Category::Separator},
    {"{",    Token::LBrace,    Category::Tokenizable | Category::Separator},
    {"}",    Token::RBrace,    Category::Tokenizable | Category::Separator},
    {"[",    Token::LBracket,  Category::Tokenizable | Category::Separator},
    {"]",    Token::RBracket,  Category::Tokenizable | Category::Separator},
    {":",    Token::Colon,     Category::Tokenizable | Category::Separator},
    {";",    Token::Semi,      Category::Tokenizable | Category::Separator},
    {",",    Token::Comma,     Category::Tokenizable | Category::Separator | Category::Operator},
    {".",    Token::Dot,       Category::Tokenizable | Category::Separator | Category::Operator},
    
    // Operators
    {"+",  Token::Add,         Category::Tokenizable | Category::Operator},
    {"-",  Token::Sub,         Category::Tokenizable | Category::Operator},
    {"=",  Token::Assign,      Category::Tokenizable | Category::Operator},
    {"*",  Token::Mul,         Category::Tokenizable | Category::Operator},
    {"/",  Token::Div,         Category::Tokenizable | Category::Operator},
    {"++", Token::Increment,   Category::Tokenizable | Category::Operator},
    {"+=", Token::AddEq,       Category::Tokenizable | Category::Operator},
    {"-=", Token::SubEq,       Category::Tokenizable | Category::Operator},
    {"*=", Token::MulEq,       Category::Tokenizable | Category::Operator},
    {"/=", Token::DivEq,       Category::Tokenizable | Category::Operator},
    {"==", Token::Equals,      Category::Tokenizable | Category::Operator},
    {"<",  Token::Lesser,      Category::Tokenizable | Category::Operator},
    {">",  Token::Greater,     Category::Tokenizable | Category::Operator},
    {"<=", Token::LessEq,      Category::Tokenizable | Category::Operator},
    {">=", Token::GreatEq,     Category::Tokenizable | Category::Operator},
    {"||", Token::LogOr,       Category::Tokenizable | Category::Operator},
    {"&&", Token::LogAnd,      Category::Tokenizable | Category::Operator},
    {"!",  Token::LogNot,      Category::Tokenizable | Category::Operator},
    {"|",  Token::VBar,        Category::Tokenizable | Category::Operator},
    {"^",  Token::Caret,       Category::Tokenizable | Category::Operator},
    {"&",  Token::Ampersand,   Category::Tokenizable | Category::Operator},
    {"~",  Token::Tilde,       Category::Tokenizable | Category::Operator},
    
    {"string",      Token::String,     Category::Literal | Category::Operand},
    {"integer",     Token::Integer,    Category::Literal | Category::Operand},
    {"character",   Token::Character,  Category::Literal | Category::Operand},
    {"ipv4",        Token::Ipv4,       Category::Literal | Category::Operand},
    {"color",       Token::Color,      Category::Literal | Category::Operand},
});

static constexpr auto MaxLen = max_len(extract_key(reserved, &TokenInfo::str));

const auto tokenize (const char *src) {
    uint pos = 0, max = 0;
    for (uint i = 1; i < reserved.size(); i++) {
        auto &entry = reserved[i];
        if (!(entry.categories & Category::Tokenizable)) {
            continue;
        }
        const auto &str = entry.str;
        if (max > str.size()) {
            continue;
        }
        if (!strncmp(str.data(), src, str.size())) {
            pos = i;
            max = str.size();
        }
    }
    return reserved[pos];
}

const auto check_type (const std::string &str) {
    static constexpr auto table = extract_pair(reserved, &TokenInfo::str, &TokenInfo::type);
    static const auto map = std::unordered_map(table.begin(), table.end());
    if (str.size() > MaxLen) {
        return Token::Type::Unknown;
    }
    auto it = map.find(str);
    if (it == map.end()) {
        return Token::Type::Unknown;
    }
    return it->second;
}

uint32_t categorize (const Token::Type type) {
    static const auto map = []{
        std::unordered_map<Token::Type, uint32_t> m;
        for (const auto &entry: reserved) {
            m.emplace(entry.type, entry.categories);
        }
        return m;
    }();
    auto it = map.find(type);
    if (it == map.end()) {
        return 0;
    }
    return it->second;
}

constexpr std::string_view to_string (const Token::Type type) {
    static constexpr auto table = extract_pair(reserved, &TokenInfo::type, &TokenInfo::str);
    static const auto map = std::unordered_map(table.begin(), table.end());
    auto it = map.find(type);
    if (it == map.end()) {
        return reserved[0].str;
    }
    return it->second;
}

}