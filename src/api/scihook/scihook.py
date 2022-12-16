import functools
import scihook._scihook as sh

__all__ = ('register', 'register_scihook', 'define_event', 'get_base_events', 'SciHookEvent')

def register(event):
  def wrapped(func):
    if isinstance(event, str):
      event_name = event
    else:
      event_name = event.__qualname__
    @functools.wraps(func)
    def new_func(context):
      func(context)
    sh.register(event_name, new_func)
    new_func.stop = lambda : sh.stop(event_name, new_func)
    return new_func
  return wrapped

def register_scihook(func, event):
  register(event)(func)

def define_event(event, triggering_events):
  if isinstance(event, str):
    event_name = event
  else:
    event_name = event.__qualname__
  sh.register_complex_event(event_name, triggering_events)

def get_base_events():
  return sh.get_base_events()

class SciHookEvent:
  pass
