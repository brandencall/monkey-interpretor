#include "object/Array.h"
#include "object/object.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace object {

std::string Array::inspect() const {
    std::ostringstream oss;
    std::string elementStrings;
    for (size_t i = 0; i < elements.size(); i++) {
        if (i == elements.size() - 1) {
            elementStrings += elements[i]->inspect();
        } else {
            elementStrings += elements[i]->inspect() + ", ";
        }
    }
    oss << "[";
    oss << elementStrings;
    oss << "]";
    return oss.str();
}
ObjectType Array::type() const { return objectType; }
std::string Array::typeToString() const { return "ARRAY"; }

} // namespace object
