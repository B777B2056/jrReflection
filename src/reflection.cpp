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

    std::map<std::string, ClassInfo>& Object::getObjects() {
        return objects;
    }

    Attribute Object::getAttribute(const std::string& attr_name) const {
        return Attribute(*this, attr_name);
    }

    std::vector<Attribute> Object::getAttributeList() const {
        std::vector<Attribute> attrs;
        for(auto& item : ci.attributes) {
            attrs.emplace_back(*this, item.first);
        }
        return attrs;
    }

    Method Object::getMethod(const std::string& method_name) const {
        return Method(*this, method_name);
    }

    std::vector<Method> Object::getMethodList() const {
        std::vector<Method> methods;
        for(auto& item : ci.methods) {
            methods.emplace_back(*this, item.first);
        }
        return methods;
    }

    void Attribute::setValue(Variable var) {
        auto set_attr = std::any_cast<std::function<void(std::shared_ptr<Reflectable>, Variable)>>(info.set_attr);
        set_attr(instance, var);
    }

    Variable Attribute::getValue() const {
        auto get_attr = std::any_cast<std::function<Variable(std::shared_ptr<Reflectable>)>>(info.get_attr);
        return get_attr(instance);
    }

    std::map<std::string, MethodInfo>& Method::getMethods() {
        return methods;
    }
}
