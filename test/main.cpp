#include "test.h"

int main(void) {
    std::string class_name = "Test";
    std::string attr_name = "attr";
    std::string method_name = "fun";
    // Register class info
    {
        jrReflection::Registrar reg(class_name);
        reg.registCreator(&Test::createInstance);
        reg.registAttribute(attr_name, &Test::attr);
        reg.registMethod(method_name, &Test::fun);
    }
    // Create a instance by name
    jrReflection::Class trc(class_name);
    trc.constructor(6, 5.564);
    // Set and get a attribute's value
    trc.setAttribute(attr_name, 65536);
    std::cout << attr_name << " is " << trc.getAttribute<int>(attr_name) << std::endl;
    // Invoke a member function
    std::cout << "invoke ret is " << trc.invoke<int>(method_name, 'b') << std::endl;
    return 0;
}
