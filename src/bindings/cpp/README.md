# Using monilogger in your C++ app

## Defining the execution events

```cpp
MoniLogger::register_base_events({
    {"SomeEvent", 0},
    {"SomeOtherEvent", 1}
});
```

```cpp
MoniLogger::register_composite_event("SomeCompositeEvent", {"SomeEvent", "SomeOtherEvent"});
```

## Defining the exposed execution context

```cpp
struct MyExecutionContext : MoniLogger::MoniLoggerExecutionContext
{
    /*
        Declare the exposed variables here.
    */

    // Default constructor for execution context structs.
    using MoniLogger::MoniLoggerExecutionContext::MoniLoggerExecutionContext;
};
```

### Exposing local variables

```cpp
struct MyExecutionContext : MoniLogger::MoniLoggerExecutionContext
{
    // Declaring a getter for the 'foo' variable.
    const pybind11::object get_foo() const { if (foo != nullptr) return pybind11::cast(*foo); else return pybind11::cast<pybind11::none>(Py_None); }
    // Struct member storing the pointer to the local variable.
    double *foo = nullptr;

    using MoniLogger::MoniLoggerExecutionContext::MoniLoggerExecutionContext;
};
```

### Exposing class members

```cpp
struct MyExecutionContext : MoniLogger::MoniLoggerExecutionContext
{
    const pybind11::object get_foo() const { if (foo != nullptr) return pybind11::cast(*foo); else return pybind11::cast<pybind11::none>(Py_None); }
    // Declaring a getter for the 'bar' variable.
    std::vector<double> get_bar() const {return instance->bar;}

    double *foo = nullptr;
    // Struct member storing the pointer to the object whose members are exposed.
    MyClass *instance = nullptr;

    // Adapted struct constructor.
    MyExecutionContext(MyClass *instance, std::string name) : MoniLogExecutionContext(name), instance(instance) {}
    virtual ~MyExecutionContext() = default;
};
```

### Exposing the context as a Python class

```cpp
// Initialization function for the interface module.
std::function<void (pybind11::module_, pybind11::object)> interface_module_initializer = [](
        // Interface module containing the exposed execution contexts.
        pybind11::module_ interface_module,
        // MoniLoggerExecutionContext as a Python object for subclassing.
        pybind11::object context_class) {
    // Declaring a new execution context to be exposed as a Python class.
    pybind11::class_<MyExecutionContext, std::shared_ptr<MyExecutionContext>>(interface_module, "MyExecutionContext", context_class)
        // Declaring 'foo' as an exposed variable of the context, accessible through the 'get_foo' function.
        .def_property_readonly("foo", &MyExecutionContext::get_foo)
        .def_property_readonly("bar", &IterativeHeatEquationContext::get_u_n)
        /*
            Declare additional variables to be exposed here.
        */
        // Providing some string representation for the context.
        .def("__str__", [](MyExecutionContext &self)
            {
                std::ostringstream oss;
                oss << self.name;
                return oss.str();
            })
        .def("__repr__", [](MyExecutionContext &self)
            {
                std::ostringstream oss;
                oss << self.name;
                return oss.str();
            });
    /*
        Register additional contexts here.
    */
};
```

## Registering C++ classes as Python classes

```cpp
    // ...

    pybind11::class_<BoundClass>(interface_module, "BoundClass");

    // ...
```

## Starting the Python interpreter and initializing monilogger

```cpp
MoniLogger::initialize_monilogger(python_path, python_scripts, interface_module, interface_module_initializer);
```

## Triggering execution events

```cpp
std::shared_ptr<MyExecutionContext> context(new MyExecutionContext(this, "MyCurrentContext"));
MoniLog::trigger(0, context);
MoniLog::trigger("SomeCompositeEvent", context);
```