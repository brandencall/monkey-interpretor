#pragma once
#include "object/object.h"
#include <map>
#include <memory>
#include <string>
#include <tuple>

namespace object {
class Environment {
  public:
    std::map<std::string, std::unique_ptr<Object>> store;
    Environment *outer;

    Environment() : outer(nullptr) {};
    Environment(Environment *outer) : outer(outer) {};
    ~Environment() = default;
    Object *get(std::string &name);
    Object *set(std::string name, std::unique_ptr<Object> value);
};

} // namespace object
