#define DECLARE_${'_'.join([s.upper() for s in qualified_name])}_EVENTS ${'\\'}
private: ${'\\'}
% for s in structs:
size_t ${base_event}_${s['method'].upper()}_BEFORE ${'\\'}
size_t ${base_event}_${s['method'].upper()}_AFTER ${'\\'}
% endfor


#define INITIALIZE_${'_'.join([s.upper() for s in qualified_name])}_EVENTS ${'\\'}
% for s in structs:
${base_event}_${s['method'].upper()}_BEFORE = SciHook::register_base_event("${'.'.join([s.capitalize() for s in qualified_name])}.${s['method'].capitalize()}.Before"); ${'\\'}
${base_event}_${s['method'].upper()}_AFTER = SciHook::register_base_event("${'.'.join([s.capitalize() for s in qualified_name])}.${s['method'].capitalize()}.After"); ${'\\'}
% endfor


% for s in structs:
#define TRIGGER_${base_event}_${s['method'].upper()}(...) ${'\\'}
std::shared_ptr<${s['name']}> ctx(new ${s['name']}(this, "${s['name']}"${''.join([f", &{l['name']}" for l in s['locals']])})); ${'\\'}
SciHook::trigger(${base_event}_${s['method'].upper()}_BEFORE, ctx); ${'\\'}
__VA_ARGS__ ${'\\'}
SciHook::trigger(${base_event}_${s['method'].upper()}_AFTER, ctx);


% endfor