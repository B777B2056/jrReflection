#ifndef DEF_H
#define DEF_H

#include <any>
#include <map>
#include <string>
#include <stdexcept>
#include <functional>

namespace jrReflection {
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

    /* ============================ Method(Non-member function) reflection information structure ============================ */
    struct MethodInfo {
        std::string method_name;
        std::any method;

        MethodInfo() : method_name("") {}

        template<typename R, typename... Args>
        MethodInfo(const std::string& name, R(*method)(Args...)) : method_name(name) {
            std::function<Variable(Args...)> method_wrapper = [method](Args&&... args) {
                return Variable((*method)(std::forward<Args>(args)...));
            };
            this->method = std::make_any<std::function<Variable(Args...)>>(method_wrapper);
        }
    };

    /* ============================ Class reflection information structure ============================ */
    struct Reflectable {
        virtual ~Reflectable() = default;
    };

    struct AttrInfo {
        std::string attr_name;
        std::any set_attr;
        std::any get_attr;

        AttrInfo() = default;

        template<typename T, typename Attr>
        AttrInfo(const std::string& name, Attr T::*attr) : attr_name(name) {
            std::function<void(Reflectable*, Variable)> set_attr_wrapper = [attr](Reflectable* a, Variable var) {
                T* b = dynamic_cast<T*>(a);
                if(!b) {
                    throw std::runtime_error("Reflectable is not base of ChildClass");
                }
                b->*attr = var.toType<Attr>();
                return ;
            };
            set_attr = std::make_any<std::function<void(Reflectable*, Variable)>>(set_attr_wrapper);
            std::function<Variable(Reflectable*)> get_attr_wrapper = [attr](Reflectable* a) {
                T* b = dynamic_cast<T*>(a);
                if(!b) {
                    throw std::runtime_error("Reflectable is not base of ChildClass");
                }
                return Variable(b->*attr);
             };
            get_attr = std::make_any<std::function<Variable(Reflectable*)>>(get_attr_wrapper);
        }
    };

    struct MemfunInfo {
        std::string memfun_name;
        std::any memfun;

        MemfunInfo() = default;

        template<typename T, typename R, typename... Args>
        MemfunInfo(const std::string& name, R(T::*method)(Args...)) : memfun_name(name) {
            std::function<Variable(Reflectable*, Args...)> method_wrapper = [method](Reflectable* a, Args&&... memArgs) {
                T* b = dynamic_cast<T*>(a);
                if(!b) {
                    throw std::runtime_error("Reflectable is not base of ChildClass");
                }
                return Variable((b->*method)(std::forward<Args>(memArgs)...));
            };
            this->memfun = std::make_any<std::function<Variable(Reflectable*, Args...)>>(method_wrapper);
        }
    };

    struct ClassInfo {
        std::string class_name;
        std::any creator;
        std::map<std::string, AttrInfo> attributes;
        std::map<std::string, MemfunInfo> methods;

        ClassInfo() : class_name("") {}

        template<typename T, typename... Args>
        void set_ctor() {
            std::function<Reflectable*(Args...)> ctor_wrapper = [](Args&&... ctorArgs)->Reflectable* {
                Reflectable* b = dynamic_cast<Reflectable*>(new T(std::forward<Args>(ctorArgs)...));
                if(!b) {
                    throw std::runtime_error("Reflectable is not base of ChildClass");
                }
                return b;
            };
            this->creator = std::make_any<std::function<Reflectable*(Args...)>>(ctor_wrapper);
        }
    };
}

#endif
