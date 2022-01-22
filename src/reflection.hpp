#ifndef REFLECTION_H
#define REFLECTION_H

#include "def.hpp"
#include <stdexcept>

namespace jrReflection {
    class Registrar {
    private:
        ClassInfo ci;

    private:
        static std::map<std::string, ClassInfo>& getObjects();
        static std::map<std::string, MethodInfo>& getMethods();

    public:
        Registrar(const std::string name = "") { ci.class_name = name; }
        ~Registrar();
        // Register the creator
        template<typename... Args>
        void registCreator(Reflectable*(*creator)(Args...));
        // Register the attributes
        template<typename T, typename Attr>
        void registAttribute(const std::string& attr_name, Attr T::*attr);
        // Register the member function
        template<typename T, typename R, typename... Args>
        void registMemfun(const std::string& method_name, R(T::*method)(Args...));
        // Register the non-member function
        template<typename R, typename... Args>
        void registMethod(const std::string& method_name, R(*method)(Args...));
    };

    class Method {
    private:
        std::string method_name;

    private:
        static std::map<std::string, MethodInfo>& getMethods();

    public:
        Method(const std::string& name) : method_name(name) {}
        // Invoke a nonmember function
        template<typename R, typename... Args>
        R invokeMethod(Args&&... args);
    };

    class Object {
    private:
        std::string class_name;
        Reflectable* instance;   // pointer to instance

    private:
        static std::map<std::string, ClassInfo>& getObjects();

    public:
        Object(const std::string& name) : class_name(name), instance(nullptr) {}
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
        R invokeMemberFunc(const std::string& method_name, Args&&... args);
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
    void Registrar::registMemfun(const std::string& method_name, R(T::*method)(Args...)) {
        ci.methods.emplace(method_name, MemfunInfo(method_name, method));
    }

    template<typename R, typename... Args>
    void Registrar::registMethod(const std::string& method_name, R(*method)(Args...)) {
        Registrar::getMethods().emplace(method_name, MethodInfo(method_name, method));
    }

    template<typename R, typename... Args>
    R Method::invokeMethod(Args&&... args) {
        auto fun = std::any_cast<std::function<R(Args...)>>(Method::getMethods()[method_name].method);
        return fun(std::forward<Args>(args)...);
    }

    template<typename... Args>
    void Object::constructor(Args&&... args) {
        typedef Reflectable* (*Ctor)(Args...);
        try {
            Ctor ctor = std::any_cast<Ctor>(Object::getObjects()[class_name].creator);
            instance = (*ctor)(std::forward<Args>(args)...);
        } catch (const std::bad_any_cast&) {
            throw std::runtime_error("Constructor is empty!");
        }
    }

    template<typename Attr>
    void Object::setAttribute(const std::string& attr_name, const Attr& new_var) {
        long offset = (Object::getObjects()[class_name].attributes)[attr_name].attr_offset;
        Attr* pvar = reinterpret_cast<Attr*>(instance + offset);
        *pvar = new_var;
    }

    template<typename Attr>
    Attr Object::getAttribute(const std::string& attr_name) const {
        long offset = (Object::getObjects()[class_name].attributes)[attr_name].attr_offset;
        Attr* pvar = reinterpret_cast<Attr*>(instance + offset);
        return *pvar;
    }

    template<typename R, typename... Args>
    R Object::invokeMemberFunc(const std::string& method_name, Args&&... args) {
        auto memfun = std::any_cast<std::function<R(Reflectable*, Args...)>>((Object::getObjects()[class_name].methods)[method_name].memfun);
        return memfun(instance, std::forward<Args>(args)...);
    }
}

#endif // CLASS_INFO_H
