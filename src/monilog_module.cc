#include "MoniLog.h"

PYBIND11_MODULE(_monilog, m) {
    m.attr("__name__") = "monilog._monilog";
	py::class_<MoniLog::MoniLogExecutionContext, std::shared_ptr<MoniLog::MoniLogExecutionContext>>(m, "MoniLogExecutionContext")
        .def(py::init<>());
    m.def("register", &MoniLog::register_monilogger);
    m.def("stop", &MoniLog::unregister_monilogger);
    m.def("define_event", &MoniLog::register_composite_event);
    m.def("define_basic_events", &MoniLog::register_base_events);
    m.def("emit_event", [](std::string event_name, std::shared_ptr<MoniLog::MoniLogExecutionContext> scope)
    {
        MoniLog::trigger(event_name, scope);
    });
}