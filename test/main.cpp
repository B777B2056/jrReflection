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
    jrReflection::Variable ret = method.invokeMethod(98);
    std::cout << ret.toType<std::string>() << std::endl;
    /* Class test */
    std::string class_name = "Test";
    std::string attr_name = "attr";
    std::string memfun_name = "fun";
    // Register class info
    {
        jrReflection::Registrar reg(class_name);
        reg.registCreator<Test, int, double>();
        reg.registAttribute(attr_name, &Test::attr);
        reg.registMemfun(memfun_name, &Test::fun);
    }
    // Create a instance by name
    jrReflection::Object object(class_name);
    object.constructor(6, 5.564);
    // Set and get a attribute's value
    int new_var = 65536;
    jrReflection::Variable attr;
    attr.toVar(new_var);
    object.setAttribute(attr_name, attr);
    std::cout << attr_name << " is " << object.getAttribute(attr_name).toType<int>() << std::endl;
    // Invoke a member function
    jrReflection::Variable mem_ret = object.invokeMemberFunc(memfun_name, 'b');
    std::cout << "invoke ret is " << mem_ret.toType<int>() << std::endl;
    return 0;
}
