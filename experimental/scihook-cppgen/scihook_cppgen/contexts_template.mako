#ifndef ${header}
#define ${header}

#include <Python.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <SciHook.h>

% for i in includes:
${i}
% endfor

<%
  segments = qualified_name
  namespace = None
  if len(segments) > 1:
    namespace = '::'.join(segments[:-1])
%>
% if namespace:
namespace ${namespace} {
  class ${segments[-1]};
}
% else:
class ${s['class']};
% endif


% for s in structs:
<% len_locals = len(s['locals']) - 1 %>\
struct ${s['name']} : SciHook::SciHookExecutionContext
{
    const pybind11::object get_instance() const { if (instance != nullptr) return pybind11::cast(instance); else return pybind11::cast<pybind11::none>(Py_None); }
    % for l in s['locals']:
    const pybind11::object get_${l['name']}() const { if (${l['name']} != nullptr) return pybind11::cast(*${l['name']}); else return pybind11::cast<pybind11::none>(Py_None); }
    % endfor

    ${'const ' if s['isConst'] else ''}${s['class']} *instance = nullptr;
    % for l in s['locals']:
    ${l['type']} *${l['name']} = nullptr;
    % endfor

    ${s['name']}(
        ${'const ' if s['isConst'] else ''}${s['class']} *instance,
        std::string name${',' if s['locals'] else ''}
        % for i, l in enumerate(s['locals']):
        ${l['type']} *${l['name']}${','*bool(len_locals - i)}
        % endfor
    ) : SciHookExecutionContext(name),
        instance(instance)${',' if s['locals'] else ''}
        % for i, l in enumerate(s['locals']):
        ${l['name']}(${l['name']})${','*bool(len_locals - i)}
        % endfor
        {}
};

% endfor
PYBIND11_EMBEDDED_MODULE(${qualified_name[-1]}, m)
{
% for s in structs:
  pybind11::class_<${s['name']}, std::shared_ptr<${s['name']}>, SciHook::SciHookExecutionContext>(m, "${s['name']}")
    .def_property_readonly("instance", &${s['name']}::get_instance)
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
