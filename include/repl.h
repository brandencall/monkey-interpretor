#pragma once

#include "lexer.h"
#include "token.h"
#include <ostream>
#include <string>
#include <vector>

namespace repl {
const std::string PROMPT = ">> ";
const std::string MONKEY_FACE = R"(
            __,__
   .--.  .-"     "-.  .--.
  / .. \/  .-. .-.  \/ .. \
 | |  '|  /   Y   \  |'  | |
 | \   \  \ 0 | 0 /  /   / |
  \ '- ,\.-"""""""-./, -' /
   ''-' /_   ^ ^   _\ '-''
       |  \._   _./  |
       \   \ '~' /   /
        '._ '-=-' _.'
           '-----'
)";
class REPL {
  public:
    static void start(std::ostream &out);

  private:
    static void printParserErrors(std::ostream &out, std::vector<std::string> errors);
};
} // namespace repl
