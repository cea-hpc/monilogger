#include <MoniLog.h>
#include <filesystem>

PYBIND11_EMBEDDED_MODULE(example_interface, m) { }

namespace fs = std::filesystem;

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
            [](pybind11::module_ iterativeheatequation_module, pybind11::object context_class) { };

    // Define base execution events emitted by the application, to which moniloggers can register.
    MoniLog::register_base_events({
        {"SomeEvent", 0},
        {"SomeOtherEvent", 1}
    });
    
    // Define a composite event, emitted when either of its triggering events are emitted.
    MoniLog::register_composite_event("SomeCompositeEvent", {"SomeEvent", "SomeOtherEvent"});

    // Instantiating the execution context accessible from Python.
    std::shared_ptr<MoniLog::MoniLogExecutionContext> ctx(new MoniLog::MoniLogExecutionContext());

    // Bootstrapping monilog, consisting mainly of starting the Python interpreter, initializing
    // the monilog module, and evaluating the provided scripts.
    MoniLog::bootstrap_monilog(python_path, python_scripts, interface_module, interface_module_initializer);

    // Emitting some base events, triggering the registered moniloggers.
    MoniLog::trigger(0, ctx);
    MoniLog::trigger(1, ctx);

    // Emitting a composite event (can also emit base event by name).
    MoniLog::trigger("SomeCompositeEvent", ctx);

}