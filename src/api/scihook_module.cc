#include "SciHook.h"

PYBIND11_MODULE(_scihook, m) {
    m.attr("__name__") = "scihook._scihook";
	py::class_<SciHook::SciHookExecutionContext, std::shared_ptr<SciHook::SciHookExecutionContext>>(m, "SciHookExecutionContext")
        .def(py::init<>())
        .def(py::init(&SciHook::create_context))
        .def_property_readonly("name", &SciHook::SciHookExecutionContext::get_name);
    m.def("register", &SciHook::register_scihook);
    m.def("stop", &SciHook::unregister_scihook);
    m.def("register_complex_event", &SciHook::register_complex_event);
    m.def("register_base_event", &SciHook::register_base_event);
    m.def("get_base_events", &SciHook::get_base_events);
    m.def("emit_event", [](std::string event_name, std::shared_ptr<SciHook::SciHookExecutionContext> scope)
    {
        SciHook::trigger(event_name, scope);
    });
    m.def("emit_event_by_id", [](size_t event_id, std::shared_ptr<SciHook::SciHookExecutionContext> scope)
    {
        SciHook::trigger(event_id, scope);
    });
}