#include "parser.h"
#include "ast/Program.h"
#include "lexer.h"

namespace parser {

parser::Parser::Parser(lexer::Lexer* lexer) : lexer_(lexer) {

}

void parser::Parser::nextToken(){
    currentToken_ = peekToken_;
    peekToken_ = lexer_->nextToken();
}

ast::Program parser::Parser::parseProgram() {
    return ast::Program();
}

} // namespace parser
