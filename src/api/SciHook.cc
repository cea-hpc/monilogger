#include <SciHook.h>

namespace py = pybind11;

namespace SciHook
{
    namespace
    {
        std::vector<std::list<py::function>> registered_scihooks;
        std::map<std::string, std::list<py::function>> event_to_scihooks;
        std::map<std::string, std::list<py::function>> pending_scihooks;
        std::map<std::string, size_t> base_events;
        std::map<std::string, std::list<std::string>> complex_events;
        std::shared_ptr<py::scoped_interpreter> guard;

        bool is_event_registered(std::string event_name)
        {
            auto base_event = base_events.find(event_name);
            auto complex_event = complex_events.find(event_name);
            return complex_event != complex_events.end() || base_event != base_events.end();
        }

        std::vector<size_t> get_event_ids(std::string event_name)
        {
            auto triggering_events = complex_events.find(event_name);
            if (triggering_events != complex_events.end())
            {
                std::vector<size_t> result;
                for (auto triggering_event_name : triggering_events->second)
                {
                    auto triggering_event_ids = get_event_ids(triggering_event_name);
                    for (size_t id : triggering_event_ids)
                    {
                        result.emplace_back(id);
                    }
                }
                return result;
            } else {
                auto event_id = base_events.find(event_name);
                if (event_id != base_events.end())
                {
                    return {event_id->second};
                } else {
                    std::cout << "No event named " << event_name << " was found." << std::endl;
                    return {};
                }
            }
        }
    }

    void register_complex_event(std::string event_name, std::list<std::string> triggering_events)
    {
        if (is_event_registered(event_name))
        {
            throw std::invalid_argument("Event " + event_name + " is already registered.");
        } else if(triggering_events.empty())
        {
            throw std::invalid_argument("Triggering events cannot be empty.");
        } else if(std::find(triggering_events.begin(), triggering_events.end(), event_name) != triggering_events.end())
        {
            throw std::invalid_argument("Triggering events cannot contain " + event_name + ".");
        } else {
            for (auto triggering_event : triggering_events)
            {
                if (!is_event_registered(triggering_event))
                {
                    throw std::invalid_argument("No event named " + triggering_event + " was found.");
                }
            }
            complex_events[event_name] = std::list<std::string>(triggering_events);

            for (auto scihook : pending_scihooks[event_name])
            {
                register_scihook(event_name, scihook);
            }
            pending_scihooks.erase(event_name);
        }
    }

    void register_complex_events(std::map<std::string, std::list<std::string>> complex_events)
    {
        for (auto execution_event : complex_events)
        {
            register_complex_event(execution_event.first, execution_event.second);
        }
    }

    size_t register_base_event(std::string event_name)
    {
        const auto i = base_events.find(event_name);
        if (i == base_events.end())
        {
            const auto j = complex_events.find(event_name);
            if (j != complex_events.end())
            {
                throw std::invalid_argument("Event " + event_name + " is already registered as a complex event.");
            } else {
                const size_t result = base_events.size();
                base_events[event_name] = result;
                registered_scihooks.emplace_back(std::list<py::function>());
                for (auto scihook : pending_scihooks[event_name])
                {
                    registered_scihooks[result].push_back(scihook);
                    event_to_scihooks[event_name].push_back(scihook);
                }
                pending_scihooks.erase(event_name);
                return result;
            }
        } else {
            return i->second;
        }
    }

    std::list<std::string> get_base_events()
    {
        std::list<std::string> result;
        for (auto evt : base_events)
        {
            result.emplace_back(evt.first);
        }
        return result;
    }

    // TODO: use std::shared_ptr(scihook)?
    void register_scihook(std::string event_name, py::function scihook)
    {
        // if (py::hasattr(scihook, "address"))
        // {
        //     auto address = scihook.attr("address");
        //     std::cout << "Callback address: " << address() << '\n';
        // } else {
        //     std::cout << "Callback has no address\n";
        // }
        // Retrieve each base event triggering this event.

        auto ids = get_event_ids(event_name);
        if (ids.empty())
        {
            // If no base event exists yet, the event name has not been declared yet,
            // add the scihook to the list of pending scihooks for that event.
            pending_scihooks[event_name].push_back(scihook);
        } else {
            std::list<py::function> event_scihooks = event_to_scihooks[event_name];
            // Make sure a scihook can only be registered once.
            if (std::find(event_scihooks.begin(), event_scihooks.end(), scihook) == event_scihooks.end())
            {
                // Add the scihook to the list of registered scihooks for this event name.
                event_to_scihooks[event_name].push_back(scihook);
                for (auto id : ids)
                {
                    // Add the scihook to the list of registered scihooks for each base event.
                    // TODO: add to pending scihooks if the base event does not exist yet.
                    registered_scihooks[id].push_back(scihook);
                }
            }
        }
    }

    void unregister_scihook(std::string event_name, py::function scihook)
    {
        auto ids = get_event_ids(event_name);
        for (auto id : ids)
        {
            std::list<py::function> scihooks = registered_scihooks[id];
            std::list<py::function>::iterator it = std::find(scihooks.begin(), scihooks.end(), scihook);
            // Can't stop an unregistered scihook.
            if (it != scihooks.end())
            {
                scihooks.erase(it);
                registered_scihooks[id] = scihooks;
            }
        }
    }

    bool has_registered_scihooks(size_t event)
    {
        return !registered_scihooks[event].empty();
    }

    std::list<py::function> get_registered_scihooks(size_t event)
    {
        return registered_scihooks[event];
    }

    void trigger(std::string event_name, std::shared_ptr<SciHookExecutionContext> context)
    {
        // FIXME?

        for (py::function scihook : event_to_scihooks[event_name])
        {
            scihook(context);
        }
    }

    void trigger(size_t event_id, std::shared_ptr<SciHookExecutionContext> context)
    {
        std::list<py::function> scihooks = registered_scihooks[event_id];
        for (py::function scihook : scihooks)
        {
            scihook(context);
        }
    }

    SciHookExecutionContext create_context(std::string name)
    {
        return SciHookExecutionContext(name);
    }

    void initialize_scihook(std::vector<std::string> python_path,
        std::vector<std::string> python_scripts,
        std::string interface_module,
        std::function<void (py::module_)> interface_module_initializer)
    {
        guard = std::shared_ptr<py::scoped_interpreter>(new py::scoped_interpreter{});

        // Initializing the path of the Python interpreter.
        py::object append_to_path = py::module_::import("sys").attr("path").attr("append");
        for (size_t i = 0; i < python_path.size(); i++)
        {
            append_to_path(python_path[i]);
        }

        // // Initializing the SciHook Python module.
        py::module_ scihookModule = py::module_::import("scihook");
        py::module_ scihookInternalModule = py::module_::import("scihook._scihook");

        // // Initializing the user-provided interface module exposing C++ variables to Python scripts.
        py::module_ interface_py_module = py::module_::import(interface_module.c_str());
        py::object ctx = (py::object) scihookInternalModule.attr("SciHookExecutionContext");
        interface_module_initializer(interface_py_module);

        // Loading the user-provided Python scripts containing scihook definitions.
        for (size_t i = 0; i < python_scripts.size(); i++)
        {
            py::module_::import(python_scripts[i].c_str());
        }
    }
}
