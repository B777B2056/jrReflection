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

    void Object::setAttribute(const std::string& attr_name, Variable new_var) {
        auto set_attr = std::any_cast<std::function<void(Reflectable*, Variable)>>((Object::getObjects()[class_name].attributes)[attr_name].set_attr);
        set_attr(instance, new_var);
    }

    Variable Object::getAttribute(const std::string& attr_name) const {
        auto get_attr = std::any_cast<std::function<Variable(Reflectable*)>>((Object::getObjects()[class_name].attributes)[attr_name].get_attr);
        return get_attr(instance);
    }
}
