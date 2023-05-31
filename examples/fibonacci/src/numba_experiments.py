from numba import types, njit
from numba.experimental.structref import _Utils, imputils
from numba.extending import intrinsic
from numba.core import cgutils

@intrinsic
def _struct_from_meminfo(typingctx, struct_type, meminfo):
    inst_type = struct_type.instance_type

    def codegen(context, builder, sig, args):
        _, meminfo = args

        st = cgutils.create_struct_proxy(inst_type)(context, builder)
        st.meminfo = meminfo
        #NOTE: Fixes sefault but not sure about it's lifecycle (i.e. watch out for memleaks)
        context.nrt.incref(builder, types.MemInfoPointer(types.voidptr), meminfo)

        return st._getvalue()

    sig = inst_type(struct_type, types.MemInfoPointer(types.voidptr))
    return sig, codegen

@intrinsic
def _meminfo_from_struct(typingctx, val):
    def codegen(context, builder, sig, args):
        [td] = sig.args
        [d] = args

        ctor = cgutils.create_struct_proxy(td)
        dstruct = ctor(context, builder, value=d)
        meminfo = dstruct.meminfo
        context.nrt.incref(builder, types.MemInfoPointer(types.voidptr), meminfo)
        # Returns the plain MemInfo
        return meminfo
        
    sig = meminfo_type(val,)
    return sig, codegen

@intrinsic
def _cast_structref(typingctx, cast_type_ref, inst_type):
    # inst_type = struct_type.instance_type
    cast_type = cast_type_ref.instance_type
    def codegen(context, builder, sig, args):
        # [td] = sig.args
        _,d = args

        ctor = cgutils.create_struct_proxy(inst_type)
        dstruct = ctor(context, builder, value=d)
        meminfo = dstruct.meminfo
        context.nrt.incref(builder, types.MemInfoPointer(types.voidptr), meminfo)

        st = cgutils.create_struct_proxy(cast_type)(context, builder)
        st.meminfo = meminfo

        return st._getvalue()
    sig = cast_type(cast_type_ref, inst_type)
    return sig, codegen

class FibonacciExecutionContextType(types.Type):
    def __init__(self):
        super(FibonacciExecutionContextType, self).__init__(name='FibonacciExecutionContext')

from numba.typed import Dict

@njit
def foo(d):
    meminfo = d[0]
    struct = _struct_from_meminfo(None, FibonacciExecutionContextType, meminfo)
    # print(struct.n)

from scihook import register

@register("Iterate")
def log(ctx):
    d = Dict.empty(types.i8,types.MemInfoPointer(types.voidptr))
    from numba.core.runtime import rtsys
    meminfo = rtsys.meminfo_new(ctx._pointer, ctx)
    d[0] = meminfo
    foo(d)

breakpoint()