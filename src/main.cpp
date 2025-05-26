#include <iostream>
#include "repl.h"

int main() {
    std::cout << "Monkey Language Interpretor" << '\n';
    repl::REPL::start(std::cout);
    return 0;
}
