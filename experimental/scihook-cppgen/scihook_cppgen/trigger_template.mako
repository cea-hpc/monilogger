#ifndef ${header}
#define ${header}

#include <${f"{include_prefix}/" if include_prefix else ''}${f"{qualified_name[-1]}ExecutionContexts.h"}>

<% len_structs = len(structs) - 1 %>\
#define DECLARE_${'_'.join([s.upper() for s in qualified_name])}_EVENTS ${'\\'}
private: ${'\\'}
% for i, s in  enumerate(structs):
  size_t ${base_event}_${s['method'].upper()}_BEFORE; ${'\\'}
  size_t ${base_event}_${s['method'].upper()}_AFTER; ${'\\'*bool(len_structs - i)}
% endfor


#define INITIALIZE_${'_'.join([s.upper() for s in qualified_name])}_EVENTS ${'\\'}
% for i, s in  enumerate(structs):
${base_event}_${s['method'].upper()}_BEFORE = SciHook::register_base_event("${'.'.join([s.capitalize() for s in qualified_name])}.${s['method'].capitalize()}.Before"); ${'\\'}
${base_event}_${s['method'].upper()}_AFTER = SciHook::register_base_event("${'.'.join([s.capitalize() for s in qualified_name])}.${s['method'].capitalize()}.After"); ${'\\'*bool(len_structs - i)}
% endfor


% for s in structs:
#define TRIGGER_${base_event}_${s['method'].upper()}_BEFORE ${'\\'}
std::shared_ptr<${s['name']}> ctx(new ${s['name']}(this, "${s['name']}"${''.join([f", &{l['name']}" for l in s['locals']])})); ${'\\'}
SciHook::trigger(${base_event}_${s['method'].upper()}_BEFORE, ctx);

#define TRIGGER_${base_event}_${s['method'].upper()}_AFTER ${'\\'}
SciHook::trigger(${base_event}_${s['method'].upper()}_AFTER, ctx);

% endfor
#endif