#include "object/Hash.h"
#include "object/object.h"
#include <sstream>
#include <string>

namespace object {

std::string Hash::inspect() const {
    std::ostringstream oss;
    oss << "{";
    for (const auto &pair : pairs) {
        oss << pair.first.object->inspect();
        oss << ": ";
        oss << pair.second.value->inspect();
        oss << ", ";
    }
    oss << "}";
    return oss.str();
}
ObjectType Hash::type() const { return objectType; }
std::string Hash::typeToString() const { return "HASH"; }

} // namespace object
