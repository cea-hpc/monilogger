#ifndef ${header}
#define ${header}

% for i in includes:
${i}
% endfor

#include <Python.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <SciHook.h>

% for s in structs:
struct ${s['name']} : SciHook::SciHookExecutionContext
{
    % for l in s['locals']:
    const pybind11::object get_${l['name']}() const { if (${l['name']} != nullptr) return pybind11::cast(*${l['name']}); else return pybind11::cast<pybind11::none>(Py_None); }
    % endfor

    ${s['class']} *instance = nullptr;
    % for l in s['locals']:
    ${l['type']} *${l['name']} = nullptr;
    % endfor

    ${s['name']}(
        ${s['class']} *instance,
        std::string name
        % for i, l in enumerate(s['locals']):
        ${l['type']} *${l['name']}${','*bool(len(s['locals']) - i - 1)}
        % endfor
    ) : SciHookExecutionContext(name), instance(instance) {}

    using SciHook::SciHookExecutionContext;
};

% endfor
PYBIND11_EMBEDDED_MODULE(${module_name}, m)
{
% for s in structs:
  pybind11::class_<${s['name']}, std::shared_ptr<${s['name']}>, SciHook::SciHookExecutionContext>(m, "${s['name']}")
    % for l in s['locals']:
    .def_property_readonly("${l['name']}", &${s['name']}::get_${l['name']})
    % endfor
    .def("__str__", [](${s['name']} &self)
    {
      std::ostringstream oss;
      oss << "[" << self.name << "]";
      return oss.str();
    })
    .def("__repr__", [](${s['name']} &self)
    {
      std::ostringstream oss;
      oss << "[" << self.name << "]";
      return oss.str();
    });
% endfor
}
#endif
