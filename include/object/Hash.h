#pragma once
#include "object/object.h"
#include <map>
#include <string>

namespace object {
struct HashPair {
    Object* key;
    Object* value;
};
class Hash : public Object {
  public:
    ObjectType objectType = ObjectType::HASH_OBJ;
    std::map<HashKey, HashPair> pairs;

    ObjectType type() const override;
    std::string inspect() const override;
    std::string typeToString() const override;
};

} // namespace object
