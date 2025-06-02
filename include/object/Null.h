#pragma once
#include "object/object.h"
#include <string>

namespace object {
class Null : public Object {
  public:
    ObjectType objectType = ObjectType::NULL_OBJ;

    ObjectType type() const override;
    std::string inspect() const override;
    std::string typeToString() const override;
};

} // namespace object
