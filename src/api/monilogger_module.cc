#include "MoniLogger.h"

PYBIND11_MODULE(_monilogger, m) {
    m.attr("__name__") = "monilogger._monilogger";
	py::class_<MoniLogger::MoniLoggerExecutionContext, std::shared_ptr<MoniLogger::MoniLoggerExecutionContext>>(m, "MoniLoggerExecutionContext")
        .def(py::init<>());
    m.def("register", &MoniLogger::register_monilogger);
    m.def("stop", &MoniLogger::unregister_monilogger);
    m.def("define_event", &MoniLogger::register_composite_event);
    m.def("define_basic_events", &MoniLogger::register_base_events);
    m.def("get_basic-events", &MoniLogger::get_base_events);
    m.def("emit_event", [](std::string event_name, std::shared_ptr<MoniLogger::MoniLoggerExecutionContext> scope)
    {
        MoniLogger::trigger(event_name, scope);
    });
}