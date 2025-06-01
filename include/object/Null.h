#pragma once
#include "object/object.h"
#include <string>

namespace object {
class Null : public Object {
  public:
    ObjectType objectType = "NULL";

    ObjectType type() const override;
    std::string inspect() const override;
};

} // namespace object
