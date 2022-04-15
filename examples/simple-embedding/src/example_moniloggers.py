from monilog import *

@register("SomeEvent")
def monilogger1(ctx):
    print("monilogger 1 triggered!")

@register("SomeOtherEvent")
def monilogger2(ctx):
    print("monilogger 2 triggered!")

@register("SomeCompositeEvent")
def monilogger3(ctx):
    print("monilogger 3 triggered!")