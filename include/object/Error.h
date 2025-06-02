#pragma once
#include "object/object.h"
#include <string>

namespace object {
class Error : public Object {
  public:
    ObjectType objectType = ObjectType::ERROR_OBJ;
    std::string message;

    Error(std::string message) : message(message) {};
    ObjectType type() const override;
    std::string inspect() const override;
    std::string typeToString() const override;
};
} // namespace object
