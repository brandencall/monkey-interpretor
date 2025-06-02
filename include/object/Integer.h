#pragma once
#include "object/object.h"
#include <string>

namespace object {
class Integer : public Object {
  public:
    ObjectType objectType = ObjectType::INTEGER_OBJ;
    int value;

    Integer(int value) : value(value) {};
    ObjectType type() const override;
    std::string inspect() const override;
    std::string typeToString() const override;
};

} // namespace object
