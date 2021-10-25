#include "test.h"

int main(int argc, char** argv) {
    std::string class_name = "Test";
    std::string attr_name = "attr";
    std::string method_name = "method";
    // Register class info
    auto reg = tiny_reflect::Registrar<Test>::getRegistrar(class_name);
    reg.registCreator(createTest);
    reg.registAttribute(attr_name, &Test::attr);
    reg.registMethod(method_name, &Test::fun);
    // Create a instance by name
    auto trc = tiny_reflect::Class<Test>::getClass(class_name);
    auto test = trc.newInstancePointer(9, 9.68);
    // Set and get a attribute's value
    trc.setAttribute(attr_name, 65536);
    std::cout << attr_name << " is " << trc.getAttribute<int>(attr_name) << std::endl;
    // Invoke a member function
    std::cout << "invoke ret is " << trc.invoke<int>(method_name, 'b') << std::endl;
    return 0;
}
