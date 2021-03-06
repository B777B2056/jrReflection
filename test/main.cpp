#include "test.h"

using namespace jrReflection;

int main(void) {
    /* Non-member function test */
    std::string method_name = "test_fun";
    // Register nonmember method info
    {
        Registrar reg;
        reg.registMethod(method_name, test_fun);
    }
    // Invoke nonmember method
    Method method(method_name);
    std::string ret = method.invoke(98);
    std::cout << method.getName() << "'s ret is " << ret << std::endl;
    /* Class test */
    std::string class_name = "Test";
    std::vector<std::string> attr_names {"attr", "attr2", "attr3"};
    std::vector<std::string> method_names {"fun", "fun2"};
    // Register class info
    // WARNING: jrReflection::Registrar MUST be deconstructed BEFORE jrReflection::Object construct!!!!
    {
        Registrar reg(class_name);
        reg.registCreator<Test, int, double>();
        reg.registAttribute(attr_names[0], &Test::attr);
        reg.registAttribute(attr_names[1], &Test::attr2);
        reg.registAttribute(attr_names[2], &Test::attr3);
        reg.registMethod(method_names[0], &Test::fun);
        reg.registMethod(method_names[1], &Test::fun2);
    }
    // Create a instance by name
    Class clazz = Class::forName(class_name);
    Constructor ctor = clazz.getConstructor();
    Object object = ctor.newInstance(6, 5.564);
    // Print all attrs' name
    {
        std::cout << class_name << "'s attributes are ";
        for(auto& attr : object.getAttributeList()) {
            std::cout << attr.getName() << " ";
        }
        std::cout << std::endl;
    }
    // Print all member methods' name
    {
        std::cout << class_name << "'s methods are ";
        for(auto& method : object.getMethodList()) {
            std::cout << method.getName() << " ";
        }
        std::cout << std::endl;
    }
    // Set and get a attribute's value
    {
        Attribute attr = object.getAttribute(attr_names[0]);
        attr.setValue(65536);
        int attr_var = attr.getValue();
        std::cout << attr_names[0] << " is " << attr_var << std::endl;
    }
    {
        Attribute attr2 = object.getAttribute(attr_names[1]);
        attr2.setValue(std::string("I'm attr2."));
        std::string attr_var = attr2.getValue();
        std::cout << attr_names[1] << " is " << attr_var << std::endl;
    }
    {
        Attribute attr3 = object.getAttribute(attr_names[2]);
        attr3.setValue(std::string("I'm attr3."));
        std::string attr_var = attr3.getValue();
        std::cout << attr_names[2] << " is " << attr_var << std::endl;
    }
    // Invoke a member function
    {
        Method memfun = object.getMethod(method_names[0]);
        int mem_ret = memfun.invoke('b');
        std::cout << memfun.getName() << "'s ret is " << mem_ret << std::endl;
    }
    {
        Method memfun = object.getMethod(method_names[1]);
        std::string mem_ret = memfun.invoke(56324);
        std::cout << memfun.getName() << "'s ret is " << mem_ret << std::endl;
    }
    return 0;
}
