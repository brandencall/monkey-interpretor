#include "object/Environment.h"
#include "object/object.h"
#include <memory>
#include <utility>

namespace object {

Object *Environment::get(std::string &name) {
    auto item = store.find(name);
    if (item != store.end()) {
        return item->second.get();
    } else {
        return nullptr;
    }
}

Object *Environment::set(std::string name, std::unique_ptr<Object> value) {
    store[name] = std::move(value);
    return store[name].get();
}

} // namespace object
