#pragma once
#include "object/object.h"
#include <string>

namespace object {
class Boolean : public Object {
  public:
    ObjectType objectType = "BOOLEAN";
    bool value;

    ObjectType type() const override;
    std::string inspect() const override;
};

} // namespace object
