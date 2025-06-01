#pragma once
#include "object/object.h"
#include <string>

namespace object {
class Integer : public Object {
  public:
    ObjectType objectType = "INTEGER";
    int value;

    ObjectType type() const override;
    std::string inspect() const override;
};

} // namespace object
