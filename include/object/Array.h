#pragma once
#include "object/object.h"
#include <memory>
#include <string>
#include <vector>

namespace object {
class Array : public Object {
  public:
    ObjectType objectType = ObjectType::ARRAY_OBJ;
    std::vector<Object *> elements;

    ObjectType type() const override;
    std::string inspect() const override;
    std::string typeToString() const override;
};

} // namespace object
