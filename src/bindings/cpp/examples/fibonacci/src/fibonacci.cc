#include "fibonacci.h"
#include "fibonacci_embedding.h"

int fibonacci(int n)
{
    auto events = MoniLogger::get_base_events();

    std::shared_ptr<FibonacciExecutionContext> ctx(new FibonacciExecutionContext(n, "FibonacciExecutionContext"));

    MoniLogger::trigger(INITIALIZE, ctx);

    int u(0), v(0), i(0), t(0);
    
    ctx->previous_number = &u;
    ctx->current_number = &v;
    ctx->iteration = &i;

    MoniLogger::trigger(ITERATE, ctx);

    v = 1;

    for(i = 1; i <= n; i++)
    {
        t = u + v;
        u = v;
        v = t;
        MoniLogger::trigger(ITERATE, ctx);
    }

    MoniLogger::trigger(FINALIZE, ctx);
    
    return v;
}
