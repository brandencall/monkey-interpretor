#pragma once
#include <string>

namespace object {

enum class ObjectType { INTEGER_OBJ, BOOLEAN_OBJ, NULL_OBJ, RETURN_VALUE, ERROR_OBJ };

class Object {
  public:
    // typedef std::string ObjectType;
    virtual ~Object() = default;
    virtual ObjectType type() const = 0;
    virtual std::string inspect() const = 0;
    virtual std::string typeToString() const = 0;
};

} // namespace object
