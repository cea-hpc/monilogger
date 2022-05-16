#ifndef __FIBONACCI_H_
#define __FIBONACCI_H_

#include "fibonacci_embedding.h"

namespace Fibonacci
{
    extern size_t Embedding::INITIALIZE_ID;
    extern size_t Embedding::ITERATE_ID;
    extern size_t Embedding::FINALIZE_ID;

    int fibonacci(int n);
}

#endif