#ifndef __MONILOG_H_
#define __MONILOG_H_

#include <fstream>
#include <iomanip>
#include <type_traits>
#include <limits>
#include <utility>
#include <cmath>
#include <stdexcept>
#include <Python.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace MoniLog
{
    struct MoniLogExecutionContext
    {
        std::string name = "MoniLogExecutionContext";

        MoniLogExecutionContext() {}
        MoniLogExecutionContext(std::string name) : name(name) {}
        virtual ~MoniLogExecutionContext() = default;
    };

    void register_composite_event(std::string event_name, std::list<std::string> triggering_events);

    void register_composite_events(std::map<std::string, std::list<std::string>> composite_events);

    void register_base_events(std::map<std::string, size_t> events);

    void clear_events();

    __attribute__((visibility("default")))
    void register_monilogger(std::string event_name, py::function monilogger);

    __attribute__((visibility("default")))
    void unregister_monilogger(std::string event_name, py::function monilogger);

    bool has_registered_moniloggers(size_t event);

    std::list<py::function> get_registered_moniloggers(size_t event);

    void trigger(std::string event_name, std::shared_ptr<MoniLogExecutionContext> scope);

    void trigger(size_t event_id, std::shared_ptr<MoniLogExecutionContext> scope);

    __attribute__((visibility("default")))
    void bootstrap_monilog(std::vector<std::string> python_path,
        std::vector<std::string> python_scripts,
        std::string interface_module,
        std::function<void (py::module_, py::object)> interface_module_initializer);
}
#endif