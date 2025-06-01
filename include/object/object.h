#pragma once
#include <string>

namespace object {
class Object {
  public:
    typedef std::string ObjectType;
    virtual ~Object() = default;
    virtual ObjectType type() const = 0;
    virtual std::string inspect() const = 0;
};

} // namespace object
