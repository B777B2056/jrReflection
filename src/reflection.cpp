#include "reflection.hpp"

namespace jrReflection {
    static std::map<std::string, ClassInfo> objects;
    static std::map<std::string, MethodInfo> methods;

    Registrar::~Registrar() {
        if(!ci.class_name.empty()) {
            Registrar::getObjects().emplace(ci.class_name, ci);
        }
    }

    std::map<std::string, ClassInfo>& Registrar::getObjects() {
        return objects;
    }

    std::map<std::string, MethodInfo>& Registrar::getMethods() {
        return methods;
    }

    std::map<std::string, MethodInfo>& Method::getMethods() {
        return methods;
    }

    std::map<std::string, ClassInfo>& Object::getObjects() {
        return objects;
    }
}
