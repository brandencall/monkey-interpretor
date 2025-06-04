#pragma once
#include "object/object.h"
#include <string>

namespace object {
class Builtin : public Object {
  public:
    ObjectType objectType = ObjectType::BUILTIN_OBJ;
    BuiltinFunction fn;

    Builtin(BuiltinFunction fn) : fn(fn) {};
    ObjectType type() const override;
    std::string inspect() const override;
    std::string typeToString() const override;
};

} // namespace object
