#include <iostream>

#include "stream.h"
#include "expression.h"

int main (int argc, char **argv) {
    if (argc != 2) {
        lain::panic("usage: {} [path]", argv[0]);
    }

    std::string path = argv[1];

    auto stream = lain::TokenStream(path);

    lain::todo("Parsing");

    return 0;
}