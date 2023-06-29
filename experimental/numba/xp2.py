from numba.core.typing import cffi_utils
from numba.core import types, typing
from numba import cfunc, carray, njit
from numba.np import numpy_support

import numpy as np

c_source = """
typedef struct _big_struct {
    int    i1;
    float  f2;
    double d3;
    float  af4[9];
} big_struct;
typedef struct _error {
    int bits:4;
} error;
typedef double (*myfunc)(big_struct*, size_t);
"""

def get_ffi(src=c_source):
    from cffi import FFI

    ffi = FFI()
    ffi.cdef(src)
    return ffi

def test_cfunc_callback():
    ffi = get_ffi()
    breakpoint()
    big_struct = ffi.typeof('big_struct')
    nb_big_struct = cffi_utils.map_type(big_struct, use_record_dtype=True)
    sig = cffi_utils.map_type(ffi.typeof('myfunc'), use_record_dtype=True)

    @njit
    def calc(base):
        tmp = 0
        for i in range(base.size):
            elem = base[i]
            tmp += elem.i1 * elem.f2 / elem.d3
            tmp += base[i].af4.sum()
        return tmp

    @cfunc(sig)
    def foo(ptr, n):
        base = carray(ptr, n)
        return calc(base)

    # Make data
    mydata = ffi.new('big_struct[3]')
    ptr = ffi.cast('big_struct*', mydata)
    for i in range(3):
        ptr[i].i1 = i * 123
        ptr[i].f2 = i * 213
        ptr[i].d3 = (1 + i) * 213
        for j in range(9):
            ptr[i].af4[j] = i * 10 + j

    # Address of my data
    addr = int(ffi.cast('size_t', ptr))
    got = foo.ctypes(addr, 3)

    # Make numpy array from the cffi buffer
    array = np.ndarray(
        buffer=ffi.buffer(mydata),
        dtype=numpy_support.as_dtype(nb_big_struct),
        shape=3,
        )
    expect = calc(array)
    print(got)
    print("==================")
    print(expect)

if __name__ == "__main__":
    test_cfunc_callback()