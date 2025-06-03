#include "object/Environment.h"
#include "object/object.h"
#include <memory>
#include <utility>

namespace object {

Object *Environment::get(std::string &name) {
    auto item = store.find(name);
    Object *result;
    if (item == store.end() && outer != nullptr) {
        result = outer->get(name);
    } else if (item == store.end()) {
        result = nullptr;
    } else {
        result = item->second.get();
    }
    return result;
}

Object *Environment::set(std::string name, std::unique_ptr<Object> value) {
    store[name] = std::move(value);
    return store[name].get();
}

} // namespace object
