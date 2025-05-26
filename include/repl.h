#pragma once

#include "lexer.h"
#include "token.h"
#include <ostream>
#include <string>

namespace repl {
const std::string PROMPT = ">> ";
class REPL {
  public:
    static void start(std::ostream &out);
};
} // namespace repl
