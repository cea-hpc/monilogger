#include "fibonacci.h"
#include "fibonacci_embedding.h"

int fibonacci(int n)
{
    std::shared_ptr<FibonacciExecutionContext> ctx(new FibonacciExecutionContext("FibonacciExecutionContext"));

    int u = 0;
    int v = 1;
    int i = 1;
    int t = 0;
    
    ctx->previous_number = &u;
    ctx->current_number = &v;
    ctx->iteration = &i;

    MoniLogger::trigger(0, ctx);

    for(i = 2; i <= n; i++)
    {
        t = u + v;
        u = v;
        v = t;
        MoniLogger::trigger(0, ctx);
    }
    return v;
}
