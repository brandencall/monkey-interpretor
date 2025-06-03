#include "repl.h"
#include "evaluator/evaluator.h"
#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <string>
#include <vector>

namespace repl {
void REPL::start(std::ostream &out) {
    std::string line;
    object::Environment *env = new object::Environment();

    while (true) {
        out << PROMPT;
        out.flush();
        std::getline(std::cin, line);
        if (line.empty()) {
            return;
        }
        auto lexer = std::make_unique<lexer::Lexer>(line);
        parser::Parser parser = parser::Parser(std::move(lexer));
        std::unique_ptr<ast::Program> program = parser.parseProgram();

        if (parser.errors()->size() != 0) {
            printParserErrors(out, *parser.errors());
            continue;
        }
        auto evaluated = evaluator::eval(program.get(), env);
        if (evaluated != nullptr) {
            out << evaluated->inspect() << '\n';
        }
    }
}
void REPL::printParserErrors(std::ostream &out, std::vector<std::string> errors) {
    out << MONKEY_FACE << '\n';
    out << "We ran into an issue!" << '\n';
    out << "parser errors:" << '\n';
    for (const auto &err : errors) {
        out << "\t" << err << "\n";
    }
}
} // namespace repl
