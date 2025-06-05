#pragma once
#include <string>
#include <vector>

namespace object {

class Object;
enum class ObjectType {
    INTEGER_OBJ,
    BOOLEAN_OBJ,
    NULL_OBJ,
    RETURN_VALUE,
    ERROR_OBJ,
    FUNCTION_OBJ,
    STRING_OBJ,
    BUILTIN_OBJ,
    ARRAY_OBJ,
    HASH_OBJ,
};
using BuiltinFunction = Object* (*)(const std::vector<Object *>& args);

class Object {
  public:
    virtual ~Object() = default;
    virtual ObjectType type() const = 0;
    virtual std::string inspect() const = 0;
    virtual std::string typeToString() const = 0;
};

} // namespace object
