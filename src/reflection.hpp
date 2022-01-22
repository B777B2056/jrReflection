#ifndef REFLECTION_H
#define REFLECTION_H

#include <any>
#include <map>
#include <string>
#include <stdexcept>
#include <functional>

namespace jrReflection {
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

    struct MethodInfo {
        std::string method_name;
        std::any method;

        MethodInfo() : method_name("") {}

        template<typename T, typename R, typename... Args>
        MethodInfo(const std::string& name, R(T::*method)(Args...))
            : method_name(name) {
            std::function<R(Reflectable*, Args...)> method_wrapper = [method](Reflectable* a, Args&&... memArgs)->R {
                                                                            T* b = dynamic_cast<T*>(a);
                                                                            return (b->*method)(std::forward<Args>(memArgs)...);
                                                                      };
            this->method = std::make_any<std::function<R(Reflectable*, Args...)>>(method_wrapper);
        }
    };

    struct ClassInfo {
        std::string class_name;
        std::any creator;
        std::map<std::string, AttrInfo> attributes;
        std::map<std::string, MethodInfo> methods;
    };

    class Registrar {
    private:
        ClassInfo ci;

    private:
        // Register the class info
        void registFinish();

    public:
        Registrar(const std::string name) { ci.class_name = name; }
        ~Registrar() { registFinish(); }
        // Register the creator
        template<typename... Args>
        void registCreator(Reflectable*(*creator)(Args...));
        // Register the attributes
        template<typename T, typename Attr>
        void registAttribute(const std::string& attr_name, Attr T::*attr);
        // Register the methods
        template<typename T, typename R, typename... Args>
        void registMethod(const std::string& method_name, R(T::*method)(Args...));
        // Unregister
        void unregist(const std::string& class_name);
    };

    class Class {
    private:
        std::string class_name;
        Reflectable* instance;   // pointer to instance

    private:
        static std::map<std::string, ClassInfo>& getMap();

    public:
        Class(const std::string& name) : class_name(name), instance(nullptr) {}
        // Create a object
        template<typename... Args>
        void constructor(Args&&... args);
        // Set a target instance's data member
        template<typename Attr>
        void setAttribute(const std::string& attr_name, const Attr& new_var);
        // Get a target instance's data member
        template<typename Attr>
        Attr getAttribute(const std::string& attr_name) const;
        // Invoke a member function
        template<typename R, typename... Args>
        R invoke(const std::string& method_name, Args&&... args);
    };

    /* Implementation */
    template<typename... Args>
    void Registrar::registCreator(Reflectable*(*creator)(Args...)) {
        ci.creator = std::make_any<Reflectable*(*)(Args...)>(creator);
    }

    template<typename T, typename Attr>
    void Registrar::registAttribute(const std::string& attr_name, Attr T::*attr) {
        ci.attributes.emplace(attr_name, AttrInfo(attr_name, attr));
    }

    template<typename T, typename R, typename... Args>
    void Registrar::registMethod(const std::string& method_name, R(T::*method)(Args...)) {
        ci.methods.emplace(method_name, MethodInfo(method_name, method));
    }

    template<typename... Args>
    void Class::constructor(Args&&... args) {
        typedef Reflectable* (*Ctor)(Args...);
        try {
            Ctor ctor = std::any_cast<Ctor>(Class::getMap()[class_name].creator);
            instance = (*ctor)(std::forward<Args>(args)...);
        } catch (const std::bad_any_cast&) {
            throw std::runtime_error("Constructor is empty!");
        }
    }

    template<typename Attr>
    void Class::setAttribute(const std::string& attr_name, const Attr& new_var) {
        long offset = (Class::getMap()[class_name].attributes)[attr_name].attr_offset;
        Attr* pvar = reinterpret_cast<Attr*>(instance + offset);
        *pvar = new_var;
    }

    template<typename Attr>
    Attr Class::getAttribute(const std::string& attr_name) const {
        long offset = (Class::getMap()[class_name].attributes)[attr_name].attr_offset;
        Attr* pvar = reinterpret_cast<Attr*>(instance + offset);
        return *pvar;
    }

    template<typename R, typename... Args>
    R Class::invoke(const std::string& method_name, Args&&... args) {
        auto pfun = std::any_cast<std::function<R(Reflectable*, Args...)>>((Class::getMap()[class_name].methods)[method_name].method);
        return pfun(instance, std::forward<Args>(args)...);
    }
}

#endif // CLASS_INFO_H
