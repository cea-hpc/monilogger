#ifndef __FIBONACCI_EMBEDDING_H_
#define __FIBONACCI_EMBEDDING_H_

#include <MoniLogger.h>

struct FibonacciExecutionContext : MoniLogger::MoniLoggerExecutionContext
{
	const pybind11::object get_previous_number() const { if (previous_number != nullptr) return pybind11::cast(*previous_number); else return pybind11::cast<pybind11::none>(Py_None); }
	const pybind11::object get_current_number() const { if (current_number != nullptr) return pybind11::cast(*current_number); else return pybind11::cast<pybind11::none>(Py_None); }
	const pybind11::object get_iteration() const { if (iteration != nullptr) return pybind11::cast(*iteration); else return pybind11::cast<pybind11::none>(Py_None); }
	
    int *previous_number = nullptr;
    int *current_number = nullptr;
    int *iteration = nullptr;
	
	using MoniLogger::MoniLoggerExecutionContext::MoniLoggerExecutionContext;
};

void initialize_embedding(std::vector<std::string> python_path, std::vector<std::string> python_scripts);

#endif