#include "reflection.hpp"

namespace jrReflection {
    static std::map<std::string, ClassInfo> classes;
    static std::map<std::string, MethodInfo> methods;

    /* Get std::map in header */
    std::map<std::string, ClassInfo>& Utils::getClasses() { return classes; }
    std::map<std::string, MethodInfo>& Utils::getMethods() { return methods; }

    /* If reflect a class, the destructor will regist the Classinfo into std::map */
    Registrar::~Registrar() {
        if(!ci.class_name.empty()) {
            Utils::getClasses().emplace(ci.class_name, ci);
        }
    }

    /* Class*/
    Class& Class::forName(const std::string& name) {
        static Class ret(name);
        return ret;
    }

    Class& Class::forName(std::string&& name) {
        static Class ret(std::move(name));
        return ret;
    }

    Constructor Class::getConstructor() const { return Constructor(class_name, Utils::getClasses().at(class_name).creator); }

    /* Object */
    Object::Object(const std::string& name, std::shared_ptr<Reflectable> instance) : ci(Utils::getClasses()[name]), instance(instance) {}

    Attribute Object::getAttribute(const std::string& attr_name) const { return Attribute(*this, attr_name); }

    std::vector<Attribute> Object::getAttributeList() const {
        std::vector<Attribute> attrs;
        for(auto& item : ci.attributes) {
            attrs.emplace_back(*this, item.first);
        }
        return attrs;
    }

    Method Object::getMethod(const std::string& method_name) const { return Method(*this, method_name); }

    std::vector<Method> Object::getMethodList() const {
        std::vector<Method> methods;
        for(auto& item : ci.methods) {
            methods.emplace_back(*this, item.first);
        }
        return methods;
    }

    /* Attribute */
    void Attribute::setValue(Variable var) {
        auto set_attr = std::any_cast<std::function<void(std::shared_ptr<Reflectable>, Variable)>>(info.set_attr);
        set_attr(instance, var);
    }

    Variable Attribute::getValue() const {
        auto get_attr = std::any_cast<std::function<Variable(std::shared_ptr<Reflectable>)>>(info.get_attr);
        return get_attr(instance);
    }

    /* Method */
    Method::Method(const std::string& name) : info(Utils::getMethods()[name]), instance(nullptr) {}
}
