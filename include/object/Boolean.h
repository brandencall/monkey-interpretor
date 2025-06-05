#pragma once
#include "object/Hashable.h"
#include "object/object.h"
#include <string>

namespace object {
class Boolean : public Object, public Hashable {
  public:
    ObjectType objectType = ObjectType::BOOLEAN_OBJ;
    bool value;

    Boolean(bool value) : value(value) {};
    ObjectType type() const override;
    std::string inspect() const override;
    std::string typeToString() const override;
    HashKey hashKey() const override;
};

} // namespace object
