#include "object/Function.h"
#include "object/object.h"
#include <sstream>
#include <string>
#include <vector>

namespace object {

std::string Function::inspect() const {
    std::ostringstream oss;
    std::string params;
    for (size_t i; i < parameters.size(); i++) {
        if (i == parameters.size() - 1){
            params += parameters[i]->toString();
        } else {
            params += parameters[i]->toString() + ", ";
        }
    }
    oss << "fn";
    oss << "(";
    oss << params;
    oss << ") {\n";
    oss << body->toString();
    oss << "\n}";
    return oss.str();
}
ObjectType Function::type() const { return objectType; }
std::string Function::typeToString() const { return "FUNCTION"; }

} // namespace object
