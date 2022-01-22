#include "test.h"

int main(void) {
    /* Non-member function test */
    std::string method_name = "test_fun";
    // Register nonmember method info
    {
        jrReflection::Registrar reg;
        reg.registMethod(method_name, test_fun);
    }
    // Invoke nonmember method
    jrReflection::Method method(method_name);
    std::cout << method.invokeMethod<std::string>(98) << std::endl;
    /* Class test */
    std::string class_name = "Test";
    std::string attr_name = "attr";
    std::string memfun_name = "fun";
    // Register class info
    {
        jrReflection::Registrar reg(class_name);
        reg.registCreator(&Test::createInstance);
        reg.registAttribute(attr_name, &Test::attr);
        reg.registMemfun(memfun_name, &Test::fun);
    }
    // Create a instance by name
    jrReflection::Object object(class_name);
    object.constructor(6, 5.564);
    // Set and get a attribute's value
    object.setAttribute(attr_name, 65536);
    std::cout << attr_name << " is " << object.getAttribute<int>(attr_name) << std::endl;
    // Invoke a member function
    std::cout << "invoke ret is " << object.invokeMemberFunc<int>(memfun_name, 'b') << std::endl;
    return 0;
}
