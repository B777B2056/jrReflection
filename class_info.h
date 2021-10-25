#ifndef CLASS_INFO_H
#define CLASS_INFO_H

#include <any>
#include <map>
#include <string>
#include <memory>
#include <utility>

namespace tiny_reflect {
    /* Containers */
    static std::map<std::string, std::any> _creator;
    static std::map<std::pair<std::string, std::string>, std::any> _attribute;
    static std::map<std::pair<std::string, std::string>, std::any> _method;

    /* Base definition */
//    class Base {
//    protected:
//        std::string _name;

//    public:
//        Base() : _name("") {}

//        void setClassName(const std::string& name) { this->_name = name; }

//        std::string getClassName() const { return this->_name; }
//    };

    /* Class definition */
    template <class T> class Registrar;

    template <class T>
    class Class {
    friend class Registrar<T>;

    private:
        std::string _name;  // Class's name
        std::shared_ptr<T> _instance;   // pointer to instance

    private:
        // Singlton
        Class(const std::string& name);

    public:
        // Create a static Class object
        static Class getClass(const std::string& name);

        // Create a target instance
        template<typename... Args>
        std::shared_ptr<T> newInstancePointer(const Args&... args);

        // Set a target instance's data member
        template<typename Attr>
        void setAttribute(const std::string& a_name, const Attr& newVar);

        // Get a target instance's data member
        template<typename Attr>
        Attr getAttribute(const std::string& a_name) const;

        // Invoke a member function
        template<typename R, typename... Args>
        R invoke(const std::string& m_name, const Args&... args);
    };

    // Register the constructs, attributes, methods
    template <class T>
    class Registrar {
    private:
        std::string _name;

    private:
        // Singlton
        Registrar(const std::string& name);

    public:
        // Create a static Registrar object
        static Registrar getRegistrar(const std::string& name);

        // Register the constructs
        void registCreator(std::any ctor);

        // Register the attributes
        void registAttribute(const std::string& a_name, std::any attr);

        // Register the methods
        void registMethod(const std::string& m_name, std::any method);
    };

    /* Class class implementation */
    template <class T>
    Class<T>::Class(const std::string& name) : _name(name) {

    }

    template <class T>
    Class<T> Class<T>::getClass(const std::string& name) {
        static Class _c(name);
        return _c;
    }

    template <class T>
    template <typename... Args>
    std::shared_ptr<T> Class<T>::newInstancePointer(const Args&... args) {
        typedef std::shared_ptr<T>(*Method)(Args...);
        Method m = std::any_cast<Method>(_creator[this->_name]);
        this->_instance = m(args...);
        return this->_instance;
    }

    template <class T>
    template<typename Attr>
    void Class<T>::setAttribute(const std::string& a_name, const Attr& newVar) {
        auto dptr = std::any_cast<Attr T::*>(_attribute[std::make_pair(this->_name, a_name)]);
        (*_instance).*dptr = newVar;
    }

    template <class T>
    template <typename Attr>
    Attr Class<T>::getAttribute(const std::string& a_name) const {
        auto dptr = std::any_cast<Attr T::*>(_attribute[std::make_pair(this->_name, a_name)]);
        return (*_instance).*dptr;
    }

    template <class T>
    template<typename Ret, typename... Args>
    Ret Class<T>::invoke(const std::string& m_name, const Args&... args) {
        auto fptr = std::any_cast<Ret(T::*)(Args...)>(_method[std::make_pair(this->_name, m_name)]);
        return ((*_instance).*fptr)(args...);
    }

    /* Registrar implementation */
    template <class T>
    Registrar<T>::Registrar(const std::string& name) : _name(name) {

    }

    template <class T>
    Registrar<T> Registrar<T>::getRegistrar(const std::string& name) {
        static Registrar _r(name);
        return _r;
    }

    template <class T>
    void Registrar<T>::registCreator(std::any ctor) {
        _creator.emplace(_name, ctor);
    }

    template <class T>
    void Registrar<T>::registAttribute(const std::string& a_name, std::any attr) {
        _attribute.emplace(std::make_pair(_name, a_name), attr);
    }

    template <class T>
    void Registrar<T>::registMethod(const std::string& m_name, std::any method) {
        _method.emplace(std::make_pair(_name, m_name), method);
    }
}

#endif // CLASS_INFO_H
