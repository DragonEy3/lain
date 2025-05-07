#include <iostream>

#include "lexer.h"

int main (int argc, char **argv) {
    if (argc != 2) {
        lain::panic("usage: {} [path]", argv[0]);
    }

    std::string path = argv[1];

    auto lexer = lain::Lexer(path);

    auto &tokens = lexer.scan();

    for (const auto &token : tokens) {
        std::cout << lain::to_string(token.type) << std::endl;
    }

    return 0;
}