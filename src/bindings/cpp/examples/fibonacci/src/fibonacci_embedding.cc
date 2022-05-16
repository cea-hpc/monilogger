#include "fibonacci_embedding.h"

PYBIND11_EMBEDDED_MODULE(fibonacci_interface, m)
{
    pybind11::class_<Fibonacci::Embedding::FibonacciExecutionContext, std::shared_ptr<Fibonacci::Embedding::FibonacciExecutionContext>, MoniLogger::MoniLoggerExecutionContext>(m, "FibonacciExecutionContext")
        // Defining the properties exposed by the FibonacciExecutionContext.
        .def_property_readonly("n", &Fibonacci::Embedding::FibonacciExecutionContext::get_n)
        .def_property_readonly("current_number", &Fibonacci::Embedding::FibonacciExecutionContext::get_current_number)
        .def_property_readonly("previous_number", &Fibonacci::Embedding::FibonacciExecutionContext::get_previous_number)
        .def_property_readonly("iteration", &Fibonacci::Embedding::FibonacciExecutionContext::get_iteration)
        // Defining legible __str__ and __repr__ functions for FibonacciExecutionContext.
        .def("__str__", [](Fibonacci::Embedding::FibonacciExecutionContext &self)
            {
                std::ostringstream oss;
                oss << "[" << self.name << "]\n"
                    << "  previous number: " << self.previous_number << "\n"
                    << "  current number: " << self.current_number << "\n"
                    << "  iteration: " << self.iteration << "\n" ;
                return oss.str();
            })
        .def("__repr__", [](Fibonacci::Embedding::FibonacciExecutionContext &self)
            {
                std::ostringstream oss;
                oss << "[" << self.name << "]\n"
                    << "  previous number: " << self.previous_number << "\n"
                    << "  current number: " << self.current_number << "\n"
                    << "  iteration: " << self.iteration << "\n" ;
                return oss.str();
            });
}

namespace Fibonacci::Embedding
{

    size_t INITIALIZE_ID = MoniLogger::register_base_event(INITIALIZE);
    size_t ITERATE_ID = MoniLogger::register_base_event(ITERATE);
    size_t FINALIZE_ID = MoniLogger::register_base_event(FINALIZE);

    void initialize(std::vector<std::string> python_path, std::vector<std::string> python_scripts)
    {
        // Name of the interface module (here, an embedded module declared above) exposing the execution context of the application.
        std::string interface_module = "fibonacci_interface";

        // Bootstrapping monilogger, consisting of starting the Python interpreter, initializing
        // the monilogger module, and evaluating the provided scripts.
        MoniLogger::initialize_monilogger(python_path, python_scripts, interface_module);
    }
}
