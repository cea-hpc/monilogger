#ifndef __FIBONACCI_EMBEDDING_H_
#define __FIBONACCI_EMBEDDING_H_

#include <MoniLogger.h>

#define INITIALIZE "Initialize"
#define ITERATE "Iterate"
#define FINALIZE "Finalize"

namespace Fibonacci::Embedding
{
    extern size_t INITIALIZE_ID;
    extern size_t ITERATE_ID;
    extern size_t FINALIZE_ID;

	struct FibonacciExecutionContext : MoniLogger::MoniLoggerExecutionContext
	{
		const pybind11::object get_n() const { return pybind11::cast(n); }
		const pybind11::object get_previous_number() const { if (previous_number != nullptr) return pybind11::cast(*previous_number); else return pybind11::cast<pybind11::none>(Py_None); }
		const pybind11::object get_current_number() const { if (current_number != nullptr) return pybind11::cast(*current_number); else return pybind11::cast<pybind11::none>(Py_None); }
		const pybind11::object get_iteration() const { if (iteration != nullptr) return pybind11::cast(*iteration); else return pybind11::cast<pybind11::none>(Py_None); }

		int n;
		int *previous_number = nullptr;
		int *current_number = nullptr;
		int *iteration = nullptr;
		
		FibonacciExecutionContext(int n, std::string name) : MoniLoggerExecutionContext(name), n(n)	{}
	};

	void initialize(std::vector<std::string> python_path, std::vector<std::string> python_scripts);
}
#endif
