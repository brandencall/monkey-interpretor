#pragma once
#include "object/object.h"
#include <string>
#include <vector>

namespace object {
struct HashKey {
    ObjectType type;
    int value;
    bool operator<(const HashKey& other) const {
        if (type != other.type){
            return type < other.type;
        }
        return value < other.value;
    }
};
class Hashable {
  public:
    virtual HashKey hashKey() const = 0;
};

} // namespace object
