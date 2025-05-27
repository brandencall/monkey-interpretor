#include "parser.h"
#include "ast/Program.h"
#include "lexer.h"
#include <memory>
#include <utility>

namespace parser {

parser::Parser::Parser(std::unique_ptr<lexer::Lexer> lexer) : lexer_(std::move(lexer)) {

}

void parser::Parser::nextToken(){
    currentToken_ = peekToken_;
    peekToken_ = lexer_->nextToken();
}

std::unique_ptr<ast::Program> parser::Parser::parseProgram() {
    std::unique_ptr<ast::Program> program = std::make_unique<ast::Program>();
    return nullptr; 
}

} // namespace parser
