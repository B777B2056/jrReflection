#ifndef REFLECTION_H
#define REFLECTION_H

#include "def.hpp"
#include <vector>

namespace jrReflection {
    class Constructor;
    class Attribute;
    class Method;

    /* ========================= Registrar ========================= */
    class Registrar {
    private:
        ClassInfo ci;

    public:
        Registrar(const std::string name = "") { ci.class_name = name; }
        ~Registrar();
        // Register the creator
        template<typename T, typename... Args>
        void registCreator();
        // Register the attributes
        template<typename T, typename Attr>
        void registAttribute(const std::string& attr_name, Attr T::*attr);
        // Register the member function
        template<typename T, typename R, typename... Args>
        void registMethod(const std::string& method_name, R(T::*method)(Args...));
        // Register the non-member function
        template<typename R, typename... Args>
        void registMethod(const std::string& method_name, R(*method)(Args...));
    };

    /* ========================= Class ========================= */
    class Class {
    private:
        std::string class_name;
        Class(const std::string& name) : class_name(name) {}
        Class(std::string&& name) : class_name(name) {}

    public:
        // Create a Class by string
        static Class& forName(const std::string& name);
        static Class& forName(std::string&& name);
        // Get class's constructor
        Constructor getConstructor() const;
    };

    /* ========================= Object ========================= */
    class Object {
        friend class Attribute;
        friend class Method;
        friend class Constructor;

    private:
        ClassInfo ci;
        std::shared_ptr<Reflectable> instance;  // pointer to instance

        Object(const std::string& name, std::shared_ptr<Reflectable> instance); // Used by Constructor::newInstance

    public:
        // Get a target instance's data member
        Attribute getAttribute(const std::string& attr_name) const;
        // Get a target instance's data member list
        std::vector<Attribute> getAttributeList() const;
        // Get a target instance's member function
        Method getMethod(const std::string& method_name) const;
        // Get a target instance's member function list
        std::vector<Method> getMethodList() const;
    };

    /* ========================= Constructor ========================= */
    class Constructor {
        friend class Class;

    private:
        std::string class_name;
        std::any ctor;

        Constructor(const std::string& name, std::any ctor) : class_name(name), ctor(ctor) {}   // Used by Class::getConstructor

    public:
        // Create an object
        template<typename... Args>
        Object newInstance(Args&&... args);
    };

    /* ========================= Attribute ========================= */
    class Attribute {
    private:
        AttrInfo info;
        std::shared_ptr<Reflectable> instance;  // pointer to instance

    public:
        Attribute(const Object& obj, const std::string& name) : info((obj.ci.attributes).at(name)), instance(obj.instance) {}
        // Get current attribute name
        std::string getName() const { return info.attr_name; }
        // Set a target instance's data member
        void setValue(Variable var);
        // Get value
        Variable getValue() const;
    };

    /* ========================= Method ========================= */
    class Method {
    private:
        MethodInfo info;
        std::shared_ptr<Reflectable> instance;  // pointer to instance

    public:
        // Nonmember method init
        Method(const std::string& name);
        // Member method init
        Method(const Object& obj, const std::string& name) : info((obj.ci.methods).at(name)), instance(obj.instance) {}
        // Get current method name
        std::string getName() const { return info.method_name; }
        // Invoke function
        template<typename... Args>
        auto invoke(Args&&... args);
    };

    /* ========================= Get std::map from .cpp, user cannot see it ========================= */
    class Utils {
        friend class Registrar;
        friend class Class;
        friend class Constructor;
        friend class Object;
        friend class Method;
        static std::map<std::string, ClassInfo>& getClasses();
        static std::map<std::string, MethodInfo>& getMethods();
    };

    /* Template Implementation */
    template<typename T, typename... Args>
    void Registrar::registCreator() {
        ci.set_ctor<T, Args...>();
    }

    template<typename T, typename Attr>
    void Registrar::registAttribute(const std::string& attr_name, Attr T::*attr) {
        ci.attributes.emplace(attr_name, AttrInfo(attr_name, attr));
    }

    template<typename T, typename R, typename... Args>
    void Registrar::registMethod(const std::string& method_name, R(T::*method)(Args...)) {
        ci.methods.emplace(method_name, MethodInfo(method_name, method));
    }

    template<typename R, typename... Args>
    void Registrar::registMethod(const std::string& method_name, R(*method)(Args...)) {
        Utils::getMethods().emplace(method_name, MethodInfo(method_name, method));
    }

    template<typename... Args>
    Object Constructor::newInstance(Args&&... args) {
        try {
            return Object(class_name, std::any_cast<std::function<std::shared_ptr<Reflectable>(Args...)>>(ctor)(std::forward<Args>(args)...));
        } catch (const std::bad_any_cast&) {
            throw std::runtime_error("Constructor is empty!");
        }
    }

    template<typename... Args>
    auto Method::invoke(Args&&... args) {
        if(instance) {
            // Invoke a member function
            auto memfun = std::any_cast<std::function<Variable(std::shared_ptr<Reflectable>, Args...)>>(info.method);
            return memfun(instance, std::forward<Args>(args)...);
        } else {
            // Invoke a nonmember function
            auto fun = std::any_cast<std::function<Variable(Args...)>>(info.method);
            return fun(std::forward<Args>(args)...);
        }

    }
}

#endif // CLASS_INFO_H
