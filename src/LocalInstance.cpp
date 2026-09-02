//
// Created by littl on 2026/9/2.
//

#include "LocalInstance.h"

namespace bgl {
    LocalInstance::LocalInstance(const std::string& name) :  name_(name), path_(".minecraft/versions" + name) {}

    std::string_view LocalInstance::getName() const {
        return name_;
    }
}
