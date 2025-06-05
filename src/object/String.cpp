#include "object/String.h"
#include "object/object.h"
#include <functional>
#include <string>

namespace object {

std::string String::inspect() const { return value; }
ObjectType String::type() const { return objectType; }
std::string String::typeToString() const { return "STRING"; }

HashKey String::hashKey() const{
    std::hash<std::string> hasher;
    std::size_t hashValue = hasher(value);
    HashKey result;
    result.type = type();
    result.value = hashValue;
    return result;
}
} // namespace object
