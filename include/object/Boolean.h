#pragma once
#include "object/object.h"
#include <string>

namespace object {
class Boolean : public Object {
  public:
    ObjectType objectType = ObjectType::BOOLEAN_OBJ;
    bool value;

    Boolean(bool value) : value(value) {};
    ObjectType type() const override;
    std::string inspect() const override;
};

} // namespace object
