#include "MoniLogger.h"

PYBIND11_MODULE(_monilogger, m) {
    m.attr("__name__") = "monilogger._monilogger";
	py::class_<MoniLogger::MoniLoggerExecutionContext, std::shared_ptr<MoniLogger::MoniLoggerExecutionContext>>(m, "MoniLoggerExecutionContext")
        .def(py::init<>())
        .def(py::init(&MoniLogger::create_context))
        .def_property_readonly("name", &MoniLogger::MoniLoggerExecutionContext::get_name);
    m.def("register", &MoniLogger::register_monilogger);
    m.def("stop", &MoniLogger::unregister_monilogger);
    m.def("register_complex_event", &MoniLogger::register_complex_event);
    m.def("register_base_event", &MoniLogger::register_base_event);
    m.def("get_base_events", &MoniLogger::get_base_events);
    m.def("emit_event", [](std::string event_name, std::shared_ptr<MoniLogger::MoniLoggerExecutionContext> scope)
    {
        MoniLogger::trigger(event_name, scope);
    });
}