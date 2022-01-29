#ifndef REFLECTION_H
#define REFLECTION_H

#include "def.hpp"
#include <vector>

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

    class Attribute;
    class Method;

    class Object {
        friend class Attribute;
        friend class Method;

    private:
        ClassInfo& ci;
        std::shared_ptr<Reflectable> instance;  // pointer to instance

    private:
        static std::map<std::string, ClassInfo>& getObjects();

    public:
        Object(const std::string& name) : ci(Object::getObjects()[name]), instance(nullptr) {}
        // Create a object
        template<typename... Args>
        void construct(Args&&... args);
        // Get a target instance's data member
        Attribute getAttribute(const std::string& attr_name) const;
        // Get a target instance's data member list
        std::vector<Attribute> getAttributeList() const;
        // Get a target instance's member function
        Method getMethod(const std::string& method_name) const;
        // Get a target instance's member function list
        std::vector<Method> getMethodList() const;
    };

    class Attribute {
    private:
        AttrInfo info;
        std::shared_ptr<Reflectable> instance;  // pointer to instance

    public:
        Attribute(const Object& obj, const std::string& name) : info((obj.ci.attributes)[name]), instance(obj.instance) {}
        // Get current attribute name
        std::string getName() const { return info.attr_name; }
        // Set a target instance's data member
        void setValue(Variable var);
        // Get value
        Variable getValue() const;
    };

    class Method {
    private:
        MethodInfo info;
        std::shared_ptr<Reflectable> instance;  // pointer to instance

    private:
        static std::map<std::string, MethodInfo>& getMethods();

    public:
        // Nonmember method init
        Method(const std::string& name) : info(Method::getMethods()[name]), instance(nullptr) {}
        // Member method init
        Method(const Object& obj, const std::string& name) : info((obj.ci.methods)[name]), instance(obj.instance) {}
        // Get current method name
        std::string getName() const { return info.method_name; }
        // Invoke function
        template<typename... Args>
        auto invokeMethod(Args&&... args);
    };

    /* Implementation */
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
        Registrar::getMethods().emplace(method_name, MethodInfo(method_name, method));
    }

    template<typename... Args>
    void Object::construct(Args&&... args) {
        try {
            auto ctor = std::any_cast<std::function<std::shared_ptr<Reflectable>(Args...)>>(ci.creator);
            instance = ctor(std::forward<Args>(args)...);
        } catch (const std::bad_any_cast&) {
            throw std::runtime_error("Constructor is empty!");
        }
    }

    template<typename... Args>
    auto Method::invokeMethod(Args&&... args) {
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
