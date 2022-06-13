#include <ncs/core.hpp>

namespace ncs
{
    core::core(int argc, const char* argv[]) noexcept
        : module_{ argv[0] }
        , raw_parameters_{ argv + 1, argv + argc }
    {}

    void core::process()
    {
        params_index_ = 0;

        // search command
        bool match = search();
        // if (!match) suggest(argc, argv);
        if (!match)
        {
            // execute ncs_module command
            if (!commands_.empty() && commands_[0]->name() == "ncs_module")
            {
                ncs::input_command input_command;
                params_index_ = 1; // ignore the module name
                parse_params(input_command);
                ncs::command_executor executor{ *(commands_[0]), input_command };
                executor.process();
            }
        }
    }

} // ncs