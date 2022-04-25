#include <MoniLogger.h>
#include <filesystem>

PYBIND11_EMBEDDED_MODULE(example_interface, m) { }

namespace fs = std::filesystem;

struct SimpleExecutionContext : MoniLogger::MoniLoggerExecutionContext
{
	const pybind11::object get_foo() const { if (foo != nullptr) return pybind11::cast(*foo); else return pybind11::cast<pybind11::none>(Py_None); }
	
	double *foo = nullptr;
	
	using MoniLogger::MoniLoggerExecutionContext::MoniLoggerExecutionContext;
};

int main()
{
    // Retrieve the path where the Python scripts to use are located (can provide multiple locations).
    std::string path = fs::current_path();
    std::vector<std::string> python_path = { path + "/src/" };
    
    // Provide the Python scripts to include.
    std::vector<std::string> python_scripts = {"example_moniloggers"};

    // Name of the interface module (here, an embedded module declared above) exposing the execution context of the application.
    std::string interface_module = "example_interface";

    // Initialization function for the interface module.
    std::function<void (pybind11::module_, pybind11::object)> interface_module_initializer =
            [](pybind11::module_ interface_module, pybind11::object context_class) {
        pybind11::class_<SimpleExecutionContext, std::shared_ptr<SimpleExecutionContext>>(interface_module, "SimpleExecutionContext", context_class)
            .def_property_readonly("foo", &SimpleExecutionContext::get_foo)
            .def("__str__", [](SimpleExecutionContext &self)
                {
                    std::ostringstream oss;
                    oss << self.name;
                    return oss.str();
                })
            .def("__repr__", [](SimpleExecutionContext &self)
                {
                    std::ostringstream oss;
                    oss << self.name;
                    return oss.str();
                });
    };

    // Define base execution events emitted by the application, to which moniloggers can register.
    MoniLogger::register_base_events({
        {"SomeEvent", 0},
        {"SomeOtherEvent", 1}
    });
    
    // Define a composite event, emitted when either of its triggering events are emitted.
    MoniLogger::register_composite_event("SomeCompositeEvent", {"SomeEvent", "SomeOtherEvent"});

    // Instantiating the execution context accessible from Python.
    std::shared_ptr<SimpleExecutionContext> ctx(new SimpleExecutionContext("SimpleExecutionContext"));

    // Bootstrapping monilogger, consisting mainly of starting the Python interpreter, initializing
    // the monilogger module, and evaluating the provided scripts.
    MoniLogger::bootstrap_monilogger(python_path, python_scripts, interface_module, interface_module_initializer);

    double foo(0.0);
    ctx->foo = &foo;

    // Emitting some base events, triggering the registered moniloggers.
    MoniLogger::trigger(0, ctx);

    foo = 17.0;

    MoniLogger::trigger(1, ctx);

    foo = 42.0;

    // Emitting a composite event (can also emit base event by name).
    MoniLogger::trigger("SomeCompositeEvent", ctx);

    foo = 17.0;

}