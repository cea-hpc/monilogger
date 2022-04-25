from monilogger import *

@register("SomeEvent")
def monilogger1(ctx):
    print("monilogger 1 triggered!")
    print("foo = " + str(ctx.foo))

@register("SomeOtherEvent")
def monilogger2(ctx):
    print("monilogger 2 triggered!")
    print("foo = " + str(ctx.foo))

@register("SomeCompositeEvent")
def monilogger3(ctx):
    print("monilogger 3 triggered!")
    print("foo = " + str(ctx.foo))