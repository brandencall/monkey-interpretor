#pragma once
#include "object/Hashable.h"
#include "object/object.h"
#include <string>

namespace object {
class String : public Object, public Hashable {
  public:
    ObjectType objectType = ObjectType::STRING_OBJ;
    std::string value;

    String(std::string value) : value(value) {};
    ObjectType type() const override;
    std::string inspect() const override;
    std::string typeToString() const override;
    HashKey hashKey() const override;
};

} // namespace object
