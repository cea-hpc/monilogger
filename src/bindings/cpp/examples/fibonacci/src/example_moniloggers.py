from monilogger import *

numbers = []

@register("NewIteration")
def log(ctx):
    global numbers
    numbers.append(ctx.current_number)
    print(numbers)