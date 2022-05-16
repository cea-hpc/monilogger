#include <MoniLogger.h>

namespace py = pybind11;

namespace MoniLogger
{
    namespace
    {
        std::vector<std::list<py::function>> registered_moniloggers;
        std::map<std::string, std::list<py::function>> event_to_moniloggers;
        std::map<std::string, std::list<py::function>> pending_moniloggers;
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

            for (auto monilogger : pending_moniloggers[event_name])
            {
                register_monilogger(event_name, monilogger);
            }
            pending_moniloggers.erase(event_name);
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
                registered_moniloggers.emplace_back(std::list<py::function>());
                for (auto monilogger : pending_moniloggers[event_name])
                {
                    registered_moniloggers[result].push_back(monilogger);
                }
                pending_moniloggers.erase(event_name);
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

    // TODO: use std::shared_ptr(monilogger)?
    void register_monilogger(std::string event_name, py::function monilogger)
    {
        // Retrieve each base event triggering this event.
        auto ids = get_event_ids(event_name);
        if (ids.empty())
        {
            // If no base event exists yet, the event name has not been declared yet,
            // add the monilogger to the list of pending moniloggers for that event.
            pending_moniloggers[event_name].push_back(monilogger);
        } else {
            std::list<py::function> event_moniloggers = event_to_moniloggers[event_name];
            // Make sure a monilogger can only be registered once.
            if (std::find(event_moniloggers.begin(), event_moniloggers.end(), monilogger) == event_moniloggers.end())
            {
                // Add the monilogger to the list of registered moniloggers for this event name.
                event_to_moniloggers[event_name].push_back(monilogger);
                for (auto id : ids)
                {
                    // Add the monilogger to the list of registered moniloggers for each base event.
                    // TODO: add to pending moniloggers if the base event does not exist yet.
                    registered_moniloggers[id].push_back(monilogger);
                }
            }
        }
    }

    void unregister_monilogger(std::string event_name, py::function monilogger)
    {
        auto ids = get_event_ids(event_name);
        for (auto id : ids)
        {
            std::list<py::function> moniloggers = registered_moniloggers[id];
            std::list<py::function>::iterator it = std::find(moniloggers.begin(), moniloggers.end(), monilogger);
            // Can't stop an unregistered monilogger.
            if (it != moniloggers.end())
            {
                moniloggers.erase(it);
                registered_moniloggers[id] = moniloggers;
            }
        }
    }

    bool has_registered_moniloggers(size_t event)
    {
        return !registered_moniloggers[event].empty();
    }

    std::list<py::function> get_registered_moniloggers(size_t event)
    {
        return registered_moniloggers[event];
    }

    void trigger(std::string event_name, std::shared_ptr<MoniLoggerExecutionContext> context)
    {
        for (py::function monilogger : event_to_moniloggers[event_name])
        {
            monilogger(context);
        }
    }

    void trigger(size_t event_id, std::shared_ptr<MoniLoggerExecutionContext> context)
    {
        std::list<py::function> moniloggers = registered_moniloggers[event_id];
        for (py::function monilogger : moniloggers)
        {
            monilogger(context);
        }
    }

    MoniLoggerExecutionContext create_context(std::string name)
    {
        return MoniLoggerExecutionContext(name);
    }

    void initialize_monilogger(std::vector<std::string> python_path,
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

        // // Initializing the MoniLogger Python module.
        py::module_ moniloggerModule = py::module_::import("monilogger");
        py::module_ moniloggerInternalModule = py::module_::import("monilogger._monilogger");

        // // Initializing the user-provided interface module exposing C++ variables to Python scripts.
        py::module_ interface_py_module = py::module_::import(interface_module.c_str());
        py::object ctx = (py::object) moniloggerInternalModule.attr("MoniLoggerExecutionContext");
        interface_module_initializer(interface_py_module);

        // Loading the user-provided Python scripts containing monilogger definitions.
        for (size_t i = 0; i < python_scripts.size(); i++)
        {
            py::module_::import(python_scripts[i].c_str());
        }
    }
}
