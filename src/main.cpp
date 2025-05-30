#include "repl.h"
#include <iostream>

int main() {
    std::cout << "Monkey Language Interpretor" << '\n';
    repl::REPL::start(std::cout);
    return 0;
}
