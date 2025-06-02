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

} // namespace object
