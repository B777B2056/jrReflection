#ifndef DEF_H
#define DEF_H

#include <any>
#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <functional>

namespace jrReflection {
    struct Reflectable {
        virtual ~Reflectable() = default;
    };

    class Variable {
    private:
        std::any var;

    public:
        Variable() = default;

        template<typename T>
        Variable(T t) : var(std::make_any<T>(t)) {}

        template<typename T>
        void toVar(T t) { var = std::make_any<T>(t); }

        template<typename T>
        T toType() { return std::any_cast<T>(var); }

        template<typename T>
        operator T() { return std::any_cast<T>(var); }
    };

    /* ============================ Reflection information structure ============================ */
    struct AttrInfo {
        std::string attr_name;
        std::any set_attr;
        std::any get_attr;

        AttrInfo() = default;

        template<typename T, typename Attr>
        AttrInfo(const std::string& name, Attr T::*attr) : attr_name(name) {
            std::function<void(std::shared_ptr<Reflectable>, Variable)> set_attr_wrapper = [attr](std::shared_ptr<Reflectable> a, Variable var) {
                std::shared_ptr<T> b = std::dynamic_pointer_cast<T>(a);
                if(!b) {
                    throw std::runtime_error("Reflectable is not base of ChildClass");
                }
                b.get()->*attr = var.toType<Attr>();
                return ;
            };
            set_attr = std::make_any<std::function<void(std::shared_ptr<Reflectable>, Variable)>>(set_attr_wrapper);
            std::function<Variable(std::shared_ptr<Reflectable>)> get_attr_wrapper = [attr](std::shared_ptr<Reflectable> a) {
                std::shared_ptr<T> b = std::dynamic_pointer_cast<T>(a);
                if(!b) {
                    throw std::runtime_error("Reflectable is not base of ChildClass");
                }
                return Variable(b.get()->*attr);
             };
            get_attr = std::make_any<std::function<Variable(std::shared_ptr<Reflectable>)>>(get_attr_wrapper);
        }
    };

    struct MethodInfo {
        std::string method_name;
        std::any method;

        MethodInfo() : method_name("") {}

        /* Method(Non-member function) reflection information structure init */
        template<typename R, typename... Args>
        MethodInfo(const std::string& name, R(*method)(Args...)) : method_name(name) {
            std::function<Variable(Args...)> method_wrapper = [method](Args&&... args) {
                return Variable((*method)(std::forward<Args>(args)...));
            };
            this->method = std::make_any<std::function<Variable(Args...)>>(method_wrapper);
        }

        /* Method(Member function) reflection information structure init */
        template<typename T, typename R, typename... Args>
        MethodInfo(const std::string& name, R(T::*method)(Args...)) : method_name(name) {
            std::function<Variable(std::shared_ptr<Reflectable>, Args...)> method_wrapper = [method](std::shared_ptr<Reflectable> a, Args&&... memArgs) {
                std::shared_ptr<T> b = std::dynamic_pointer_cast<T>(a);
                if(!b) {
                    throw std::runtime_error("Reflectable is not base of ChildClass");
                }
                return Variable((b.get()->*method)(std::forward<Args>(memArgs)...));
            };
            this->method = std::make_any<std::function<Variable(std::shared_ptr<Reflectable>, Args...)>>(method_wrapper);
        }
    };

    struct ClassInfo {
        std::string class_name;
        std::any creator;
        std::map<std::string, AttrInfo> attributes;
        std::map<std::string, MethodInfo> methods;

        ClassInfo() : class_name("") {}

        template<typename T, typename... Args>
        void set_ctor() {
            std::function<std::shared_ptr<Reflectable>(Args...)> ctor_wrapper = [](Args&&... ctorArgs) {
                std::shared_ptr<Reflectable> b = std::dynamic_pointer_cast<Reflectable>(std::make_shared<T>(std::forward<Args>(ctorArgs)...));
                if(!b) {
                    throw std::runtime_error("Reflectable is not base of ChildClass");
                }
                return b;
            };
            this->creator = std::make_any<std::function<std::shared_ptr<Reflectable>(Args...)>>(ctor_wrapper);
        }
    };
}

#endif
