#pragma once

#include <unordered_map>
#include <variant>
#include <vector>
#include <string_view>

#include "memory.h"
#include "stream.h"
#include "utils.h"

namespace lain {

enum Operator {
    Address,
    Dereference,
    BinaryAdd,
    OpenParen,
    CloseParen,
    Comma
};

struct OperatorInfo {
    Operator op;

    Token::Type token;
    
    std::string_view name;

    enum Category {
        RBinary,
        LBinary,
        Postfix,
        Prefix,
        Atom,
    } type;

    uint precedence;
};

constexpr OperatorInfo OperatorTable[] = {
    {Operator::Address,        Token::Ampersand,     "address",      OperatorInfo::Prefix, 10},
    {Operator::Dereference,    Token::Mul,           "dereference",  OperatorInfo::Prefix, 10},
    {Operator::BinaryAdd,      Token::Add,           "binary add",   OperatorInfo::LBinary, 1},
    {Operator::Comma,          Token::Comma,         "comma",        OperatorInfo::LBinary, 1},
};

const OperatorInfo *get_unary_mode_op (const Token::Type type) {
    static const auto map = []{
        std::unordered_map<Token::Type, const OperatorInfo*> m;
        for (const auto &entry: OperatorTable) {
            if (entry.type == OperatorInfo::Category::Prefix) {
                m.emplace(entry.token, &entry);
            }
        }
        return m;
    }();
    auto it = map.find(type);
    if (it == map.end()) {
        return nullptr;
    }
    return it->second;
}

struct Expression;

using Binary = std::pair<uptr<Expression>, uptr<Expression>>;
using List = std::vector<uptr<Expression>>;

using Value = std::variant <
    Binary,
    List,
    uptr<Expression>,
    std::size_t,
    std::string,
    std::nullptr_t
>;

struct Expression {
    enum Type {
        String,
        Integer,
        Character,
        Identifier,
        List,
        Null,
        Call,
    } type;

    Value value;

    Expression (const Token &token);

    Expression (lain::List list) : type(List), value(std::move(list)) {}

    Expression (Type type, Expression *lhs, Expression *rhs) : type(type), value(Binary{lhs, rhs}) {}
};

Expression::Expression (const Token &token) {
    switch (token.type) {
    case Token::String:
        type = Expression::String;
        value = token.str;
        break;
    case Token::Integer:
        type = Expression::Integer;
        value = token.num;
        break;
    case Token::Identifier:
        type = Expression::Identifier;
        value = token.str;
        break;
    default:
        panic("Cannot express value type {}", to_string(token.type));
        break;
    }
}

class ExpressionParser {
    TokenStream &stream;

    std::vector<Expression*> operands;
    std::vector<Operator> operators;

    enum class State {
        Binary,
        Unary,
    } state = State::Unary;

    void pop_and_push_bin (Expression::Type type, Expression *rhs) {
        // TODO: Child expr order could impact left-right associative, investigate.
        if (operands.empty()) {
            panic("Invalid expression reduction");
        }
        auto bin = alloc<Expression>(type, operands.back(), rhs);
        operands.pop_back();
        operands.push_back(bin);
    }
    
public:

    ExpressionParser(TokenStream& stream) : stream(stream) {}

    Expression *parse ();
};

Expression *ExpressionParser::parse () {
    while (!stream.done()) {
        auto &token = stream.peek();

        auto cat = categorize(token.type);

        if (cat & Category::Operand) {
            auto expr = alloc<Expression>(token);
            operands.push_back(expr);
            state = State::Binary;
        } else if (cat & Category::Operator) {
            if (state == State::Unary) {
                if (token.type == Token::LParen) {
                    operators.push_back(Operator::OpenParen);
                } else if (token.type == Token::RParen) {
                    if (stream.peek(-1).type != Token::Identifier) {
                        stream.syntax_error("Unexpected ) in unary state");
                    }
                    pop_and_push_bin(Expression::Call, alloc<Expression>(List()));
                } else {
                    auto opinfo = get_unary_mode_op(token.type);
                    if (!opinfo) {
                        stream.syntax_error("Unexpected non-prefix operator");
                    }
                    operators.push_back(opinfo->op);
                    state = State::Binary;
                }
            } else {
                todo("Binary state");
            }

        } else if (cat & Category::Separator) {
            todo("Non-operator separator {} encountered in expression", to_string(token.type));
        } else {
            stream.syntax_error("Unexpected token {} in expression.", to_string(token.type));
        }
    }
    return nullptr;
}

}