#include "class_info.h"

namespace tiny_reflect {
    void Base::setClassName(const std::string& name) {
        this->_name = name;
    }

    std::string Base::getClassName() {
        return this->_name;
    }

    Class Class::getInstance() {
        static Class _c;
        return _c;
    }

    void Class::registCreator(const std::string& name, const std::function<std::shared_ptr<void>()>& f) {
        this->_m.emplace(name, f);
    }

    std::shared_ptr<void> Class::newInstanceByName(const std::string& name) {
        return this->_m[name]();
    }

}
