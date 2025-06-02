#pragma once
#include "object/object.h"
#include <string>

namespace object {
class ReturnValue : public Object {
  public:
    ObjectType objectType = ObjectType::RETURN_VALUE;
    object::Object *value;

    ReturnValue(object::Object *value) : value(value) {};
    ObjectType type() const override;
    std::string inspect() const override;
    std::string typeToString() const override;
};
} // namespace object
