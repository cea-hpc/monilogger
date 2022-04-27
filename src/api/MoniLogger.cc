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
        std::map<std::string, std::list<std::string>> composite_events;
        std::shared_ptr<py::scoped_interpreter> guard;

        bool is_event_registered(std::string event_name)
        {
            auto base_event = base_events.find(event_name);
            auto composite_event = composite_events.find(event_name);
            return composite_event != composite_events.end() || base_event != base_events.end();
        }

        std::vector<size_t> get_event_ids(std::string event_name)
        {
            auto triggering_events = composite_events.find(event_name);
            if (triggering_events != composite_events.end())
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

    // /**
    //  * @brief Registers the event if it was not registered already.
    //  * @throws std::invalid_argument If the event is registered as a composite event.
    //  * 
    //  * @param event_name name of the base event to register.
    //  * @return size_t id already associated to the event, whether the event was
    //  * already registered or not.
    //  */
    // size_t register_base_event(std::string event_name)
    // {
    //     if (composite_events.find(event_name) == composite_events.end())
    //     {
    //         auto id = base_events.find(event_name);
    //         if (id == base_events.end())
    //         {
    //             size_t event_id;
    //             // If the event is not yet registered
    //             if (available_event_ids.empty())
    //             {
    //                 // If no event id is available, compute a fresh one
    //                 event_id =  = base_events.size()
    //                 base_events[event_name] = event_id;
    //                 // Add the corresponding list of moniloggers
    //                 registered_moniloggers.emplace_back(std::list<py::function>());

    //             } else {
    //                 // Otherwise, use the oldest available id
    //                 event_id = available_event_ids.pop_front();
    //                 base_events[event_name] = event_id;
    //                 // Clear the corresponding list of moniloggers (just in case...)
    //                 registered_moniloggers[id].clear();
    //             }
    //             return id;
    //         }
    //     } else {
    //         throw std::invalid_argument(event_name + "is already registered as a composite event.");
    //     }
    // }

    /**
     * @brief Registers the event as a composite event triggered by the provided list of events.
     * @throws std::invalid_argument If the event to register already exists, or if any of the
     * listed triggering events does not exist.
     * 
     * @param event_name name of the composite event to register.
     * @param triggering_events events (base or composite) triggering the composite event to register.
     */
    void register_composite_event(std::string event_name, std::list<std::string> triggering_events)
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
            composite_events[event_name] = std::list<std::string>(triggering_events);

            for (auto monilogger : pending_moniloggers[event_name])
            {
                register_monilogger(event_name, monilogger);
            }
            pending_moniloggers.erase(event_name);
        }
    }

    void register_composite_events(std::map<std::string, std::list<std::string>> composite_events)
    {
        for (auto execution_event : composite_events)
        {
            register_composite_event(execution_event.first, execution_event.second);
        }
    }

    void register_base_events(std::map<std::string, size_t> events)
    {
        size_t size(0);
        for (auto it = events.begin(); it != events.end(); ++it)
        {
            size = std::max(size, it->second);
        }
        size++;

        registered_moniloggers.reserve(size);
        for (size_t i = 0; i < size; i++)
        {
            registered_moniloggers.emplace_back(std::list<py::function>());
        }

        base_events = std::map<std::string, size_t>(events);

        for (auto const& [event_name, val] : base_events)
        {
            for (auto monilogger : pending_moniloggers[event_name])
            {
                register_monilogger(event_name, monilogger);
            }
            pending_moniloggers.erase(event_name);
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

    void clear_events()
    {
        base_events.clear();
        composite_events.clear();
        registered_moniloggers.clear();
    }

    void register_monilogger(std::string event_name, py::function monilogger)
    {
        // Retrieve each base event triggering this event.
        auto ids = get_event_ids(event_name);
        if (ids.empty())
        {
            // If no base event exists yet, the event name has not been declared yet,
            // add the monilogger to the list of pending moniloggers for that event.
            pending_moniloggers[event_name].emplace_back(monilogger);
        } else {
            std::list<py::function> event_moniloggers = event_to_moniloggers[event_name];
            // Make sure a monilogger can only be registered once.
            if (std::find(event_moniloggers.begin(), event_moniloggers.end(), monilogger) == event_moniloggers.end())
            {
                // Add the monilogger to the list of registered moniloggers for this event name.
                event_to_moniloggers[event_name].emplace_back(monilogger);
                for (auto id : ids)
                {
                    // Add the monilogger to the list of registered moniloggers for each base event.
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

    void initialize_monilogger(std::vector<std::string> python_path,
        std::vector<std::string> python_scripts,
        std::string interface_module,
        std::function<void (py::module_, py::object)> interface_module_initializer)
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
        interface_module_initializer(interface_py_module, ctx);

        // Loading the user-provided Python scripts containing monilogger definitions.
        for (size_t i = 0; i < python_scripts.size(); i++)
        {
            py::module_::import(python_scripts[i].c_str());
        }
    }
}
