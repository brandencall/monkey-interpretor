#include "object/Boolean.h"
#include "object/object.h"
#include <string>

namespace object {

std::string Boolean::inspect() const { 
    if (value){
        return "true";
    } else if (!value){
        return "false";
    }
    return std::to_string(value); 
}
ObjectType Boolean::type() const { return objectType; }

std::string Boolean::typeToString() const { return "BOOLEAN"; }

HashKey Boolean::hashKey(){
    HashKey result;
    result.type = type();
    result.object = this;
    if (value){
        result.value = 1;
    } else {
        result.value = 0;
    }
    return result;
}
bool Boolean::hashable() const { return true; }

} // namespace object
