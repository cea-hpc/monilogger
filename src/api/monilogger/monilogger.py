import functools
import monilogger._monilogger as mnlg

__all__ = ('register', 'register_monilogger', 'define_event')

def register(event):
  def wrapped(func):
    if isinstance(event, str):
      event_name = event
    else:
      event_name = event.__qualname__
    @functools.wraps(func)
    def new_func(context):
      func(context)
    mnlg.register(event_name, new_func)
    new_func.stop = lambda : mnlg.stop(event_name, new_func)
    return new_func
  return wrapped

def register_monilogger(func, event):
  register(event)(func)

def define_event(event, triggering_events):
  if isinstance(event, str):
    event_name = event
  else:
    event_name = event.__qualname__
  mnlg.define_event(event_name, triggering_events)
