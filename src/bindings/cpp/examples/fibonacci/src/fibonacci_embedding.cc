#include "fibonacci_embedding.h"

PYBIND11_EMBEDDED_MODULE(fibonacci_interface, m) { }

void initialize_embedding(std::vector<std::string> python_path, std::vector<std::string> python_scripts)
{
    // Name of the interface module (here, an embedded module declared above) exposing the execution context of the application.
    std::string interface_module = "fibonacci_interface";

    // Initialization function for the interface module.
    std::function<void (pybind11::module_, pybind11::object)> interface_module_initializer =
            [](pybind11::module_ interface_module, pybind11::object context_class) {
        pybind11::class_<FibonacciExecutionContext, std::shared_ptr<FibonacciExecutionContext>>(interface_module, "FibonacciExecutionContext", context_class)
            // Defining the properties exposed by the FibonacciExecutionContext.
            .def_property_readonly("n", &FibonacciExecutionContext::get_n)
            .def_property_readonly("current_number", &FibonacciExecutionContext::get_current_number)
            .def_property_readonly("previous_number", &FibonacciExecutionContext::get_previous_number)
            .def_property_readonly("iteration", &FibonacciExecutionContext::get_iteration)
            // Defining legible __str__ and __repr__ functions for FibonacciExecutionContext.
            .def("__str__", [](FibonacciExecutionContext &self)
                {
                    std::ostringstream oss;
                    oss << "[" << self.name << "]\n"
                        << "  previous number: " << self.previous_number << "\n"
                        << "  current number: " << self.current_number << "\n"
                        << "  iteration: " << self.iteration << "\n" ;
                    return oss.str();
                })
            .def("__repr__", [](FibonacciExecutionContext &self)
                {
                    std::ostringstream oss;
                    oss << "[" << self.name << "]\n"
                        << "  previous number: " << self.previous_number << "\n"
                        << "  current number: " << self.current_number << "\n"
                        << "  iteration: " << self.iteration << "\n" ;
                    return oss.str();
                });
    };

    // Define base execution events emitted by the application, to which moniloggers can register.
    MoniLogger::register_base_event(INITIALIZE);
    MoniLogger::register_base_event(ITERATE);
    MoniLogger::register_base_event(FINALIZE);

    // Bootstrapping monilogger, consisting of starting the Python interpreter, initializing
    // the monilogger module, and evaluating the provided scripts.
    MoniLogger::initialize_monilogger(python_path, python_scripts, interface_module, interface_module_initializer);
}