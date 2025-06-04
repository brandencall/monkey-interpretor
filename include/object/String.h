#pragma once
#include "object/object.h"
#include <string>

namespace object {
class String : public Object {
  public:
    ObjectType objectType = ObjectType::STRING_OBJ;
    std::string value;

    String(std::string value) : value(value) {};
    ObjectType type() const override;
    std::string inspect() const override;
    std::string typeToString() const override;
};

} // namespace object
