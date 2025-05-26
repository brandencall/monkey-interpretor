#include "repl.h"
#include "lexer.h"
#include "token.h"
#include <iostream>
#include <string>

namespace repl {
void REPL::start(std::ostream &out) {
    std::string line;
    while (true) {
        out << PROMPT;
        out.flush();
        std::getline(std::cin, line);
        if (line.empty()) {
            return;
        }
        lexer::Lexer lexer(line);
        for (token::Token tok = lexer.nextToken();
             tok.type != token::TokenType::END_OF_FILE;
             tok = lexer.nextToken()) {
            out << "{Type:" << token::tokenTypeToString(tok.type)
                << " Literal:" << tok.literal << "}\n";
        }
    }
}
} // namespace repl
