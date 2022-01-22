#include "reflection.hpp"

namespace jrReflection {
    static std::map<std::string, ClassInfo> objects;

    void Registrar::registFinish() {
        jrReflection::objects.emplace(ci.class_name, ci);
    }

    void Registrar::unregist(const std::string& class_name) {
        jrReflection::objects.erase(class_name);
    }


    std::map<std::string, ClassInfo>& Class::getMap() {
        return objects;
    }
}
