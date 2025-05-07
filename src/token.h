#pragma once

#include <unordered_map>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "utils.h"

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
        Dot,

        // Operators
        Assign,     Add,        Sub,        Mul,        Div,
        Increment,  AddEq,      SubEq,      MulEq,      DivEq,
        Equals,     Lesser,     Greater,    LessEq,     GreatEq,
        BinOr,      BinXor,     BinAnd,     BinNot,
        LogOr,      LogAnd,     LogNot,

    } type = Unknown;

    std::size_t row;
    std::size_t col;
    std::size_t len;
    std::string str;
    std::size_t num;
};

enum Category : uint32_t {
    None            = 0,
    Keyword         = 1 << 0,
    Separator       = 1 << 1,
    Operator        = 1 << 2,
    Literal         = 1 << 3,
    Modifier        = 1 << 4,
    Type            = 1 << 5,
    Tokenizable     = 1 << 6,
};

constexpr auto reserved = array_make<std::pair<std::string_view, std::pair<Token::Type, uint32_t>>>({
    {"unknown",     {Token::Unknown,    None}},

    {"eof",         {Token::Eof,        None}},
    {"identifier",  {Token::Identifier, None}},

    // Keywords
    {"var",         {Token::Var,        Tokenizable | Keyword}},
    {"fun",         {Token::Fun,        Tokenizable | Keyword}},
    {"enum",        {Token::Enum,       Tokenizable | Keyword}},
    {"struct",      {Token::Struct,     Tokenizable | Keyword}},
    {"return",      {Token::Return,     Tokenizable | Keyword}},
    {"comp",        {Token::Comp,       Tokenizable | Keyword}},
    {"for",         {Token::For,        Tokenizable | Keyword}},
    {"continue",    {Token::Continue,   Tokenizable | Keyword}},
    {"break",       {Token::Break,      Tokenizable | Keyword}},
    {"if",          {Token::If,         Tokenizable | Keyword}},
    {"else",        {Token::Else,       Tokenizable | Keyword}},
    {"import",      {Token::Import,     Tokenizable | Keyword}},
    {"module",      {Token::Module,     Tokenizable | Keyword}},
    {"protected",   {Token::Protected,  Tokenizable | Keyword | Modifier}},
    {"private",     {Token::Private,    Tokenizable | Keyword | Modifier}},
    {"static",      {Token::Static,     Tokenizable | Keyword | Modifier}},
    {"const",       {Token::Const,      Tokenizable | Keyword | Modifier}},
    {"unique",      {Token::Unique,     Tokenizable | Keyword | Modifier}},
    {"unsafe",      {Token::Unsafe,     Tokenizable | Keyword | Modifier}},
    {"debug",       {Token::Debug,      Tokenizable | Keyword | Modifier}},

    // Types
    {"u8",   {Token::U8,   Tokenizable | Type}},
    {"u16",  {Token::U16,  Tokenizable | Type}},
    {"u32",  {Token::U32,  Tokenizable | Type}},
    {"u64",  {Token::U64,  Tokenizable | Type}},
    {"uint", {Token::UInt, Tokenizable | Type}},
    {"i8",   {Token::I8,   Tokenizable | Type}},
    {"i16",  {Token::I16,  Tokenizable | Type}},
    {"i32",  {Token::I32,  Tokenizable | Type}},
    {"i64",  {Token::I64,  Tokenizable | Type}},
    {"int",  {Token::Int,  Tokenizable | Type}},
    {"void", {Token::Void, Tokenizable | Type}},

    // Separators
    {"(",    {Token::LParen,    Tokenizable | Separator}},
    {")",    {Token::RParen,    Tokenizable | Separator}},
    {"{",    {Token::LBrace,    Tokenizable | Separator}},
    {"}",    {Token::RBrace,    Tokenizable | Separator}},
    {"[",    {Token::LBracket,  Tokenizable | Separator}},
    {"]",    {Token::RBracket,  Tokenizable | Separator}},
    {":",    {Token::Colon,     Tokenizable | Separator}},
    {";",    {Token::Semi,      Tokenizable | Separator}},
    {".",    {Token::Dot,       Tokenizable | Separator}},

    // Operators
    {"+",  {Token::Add,         Tokenizable | Operator}},
    {"-",  {Token::Sub,         Tokenizable | Operator}},
    {"=",  {Token::Assign,      Tokenizable | Operator}},
    {"*",  {Token::Mul,         Tokenizable | Operator}},
    {"/",  {Token::Div,         Tokenizable | Operator}},
    {"++", {Token::Increment,   Tokenizable | Operator}},
    {"+=", {Token::AddEq,       Tokenizable | Operator}},
    {"-=", {Token::SubEq,       Tokenizable | Operator}},
    {"*=", {Token::MulEq,       Tokenizable | Operator}},
    {"/=", {Token::DivEq,       Tokenizable | Operator}},
    {"==", {Token::Equals,      Tokenizable | Operator}},
    {"<",  {Token::Lesser,      Tokenizable | Operator}},
    {">",  {Token::Greater,     Tokenizable | Operator}},
    {"<=", {Token::LessEq,      Tokenizable | Operator}},
    {">=", {Token::GreatEq,     Tokenizable | Operator}},
    {"||", {Token::LogOr,       Tokenizable | Operator}},
    {"&&", {Token::LogAnd,      Tokenizable | Operator}},
    {"!",  {Token::LogNot,      Tokenizable | Operator}},
    {"|",  {Token::BinOr,       Tokenizable | Operator}},
    {"^",  {Token::BinXor,      Tokenizable | Operator}},
    {"&",  {Token::BinAnd,      Tokenizable | Operator}},
    {"~",  {Token::BinNot,      Tokenizable | Operator}},

    {"string",      {Token::String,     Literal}},
    {"integer",     {Token::Integer,    Literal}},
    {"character",   {Token::Character,  Literal}},
    {"ipv4",        {Token::Ipv4,       Literal}},
    {"color",       {Token::Color,      Literal}},
});

const auto tokenize (const char *src) {
    uint pos = 0, max = 0;
    for (uint i = 1; i < reserved.size(); i++) {
        auto &entry = reserved[i];
        if (!(entry.second.second & Tokenizable)) {
            continue;
        }
        const auto &str = entry.first;
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
    static const auto map = std::unordered_map(reserved.begin(), reserved.end());
    auto it = map.find(str);
    if (it == map.end()) {
        return Token::Unknown;
    }
    return it->second.first;
}

constexpr std::string_view to_string (const Token::Type type) {
    for (const auto& reserved: reserved) {
        if (reserved.second.first == type) {
            return reserved.first;
        }
    }
    return reserved[0].first;
}

}