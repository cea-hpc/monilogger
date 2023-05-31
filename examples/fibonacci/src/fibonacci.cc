#include "fibonacci.h"

namespace Fibonacci
{
    int fibonacci(int n)
    {

        std::shared_ptr<Embedding::FibonacciExecutionContext> ctx(new Embedding::FibonacciExecutionContext(n, "FibonacciExecutionContext"));

        SciHook::trigger(Embedding::INITIALIZE_ID, ctx);

        int u(0), v(0), i(0), t(0);
        
        ctx->previous_number = &u;
        ctx->current_number = &v;
        ctx->iteration = &i;

        SciHook::trigger(Embedding::ITERATE_ID, ctx);

        v = 1;

        for(i = 1; i <= n; i++)
        {
            t = u + v;
            u = v;
            v = t;
            SciHook::trigger(Embedding::ITERATE_ID, ctx);
        }

        SciHook::trigger(Embedding::FINALIZE_ID, ctx);
        
        return v;
    }
}
