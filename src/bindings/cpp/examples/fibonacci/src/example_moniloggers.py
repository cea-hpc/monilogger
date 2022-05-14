from monilogger import *

numbers = []

@register("Initialize")
def log(ctx):
    print("Computing fibonacci number for n = " + str(ctx.n))

@register("Iterate")
def log(ctx):
    global numbers
    numbers.append(ctx.current_number)
    print("Computed fibonacci number " + str(ctx.current_number) + " for n = " + str(ctx.iteration))

@register("Finalize")
def log(ctx):
    global numbers
    print("Fibonacci sequence for n = " + str(ctx.n) + " is " + str(numbers))