#ifndef DEF_H
#define DEF_H

#include <any>
#include <map>
#include <string>
#include <functional>

namespace jrReflection {
    /* ============================ Method(Non-member function) reflection information structure ============================ */
    struct MethodInfo {
        std::string method_name;
        std::any method;

        MethodInfo() : method_name("") {}

        template<typename R, typename... Args>
        MethodInfo(const std::string& name, R(*method)(Args...)) : method_name(name) {
            std::function<R(Args...)> method_wrapper = [method](Args&&... args)->R {
                                                            return (*method)(std::forward<Args>(args)...);
                                                       };
            this->method = std::make_any<std::function<R(Args...)>>(method_wrapper);
        }
    };

    /* ============================ Class reflection information structure ============================ */
    struct Reflectable {
        virtual ~Reflectable() = default;
    };

    struct AttrInfo {
        std::string attr_name;
        long attr_offset;

        AttrInfo() : attr_name(""), attr_offset(-1) {}

        template<typename T, typename Attr>
        AttrInfo(const std::string& name, Attr T::*attr) : attr_name(name), attr_offset(reinterpret_cast<long>(*(void**)(&attr))) {}
    };

    struct MemfunInfo {
        std::string memfun_name;
        std::any memfun;

        MemfunInfo() : memfun_name("") {}

        template<typename T, typename R, typename... Args>
        MemfunInfo(const std::string& name, R(T::*method)(Args...)) : memfun_name(name) {
            std::function<R(Reflectable*, Args...)> method_wrapper = [method](Reflectable* a, Args&&... memArgs)->R {
                                                                            T* b = dynamic_cast<T*>(a);
                                                                            return (b->*method)(std::forward<Args>(memArgs)...);
                                                                      };
            this->memfun = std::make_any<std::function<R(Reflectable*, Args...)>>(method_wrapper);
        }
    };

    struct ClassInfo {
        std::string class_name;
        std::any creator;
        std::map<std::string, AttrInfo> attributes;
        std::map<std::string, MemfunInfo> methods;
    };
}

#endif
