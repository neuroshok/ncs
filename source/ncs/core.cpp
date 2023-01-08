#include "ncs/commands.hpp"
#include <ncs/command/executor.hpp>
#include <ncs/core.hpp>
#include <ncs/input/parameter.hpp>

namespace ncs
{
    core::core(int argc, const char* argv[]) noexcept
        : module_name_{ argv[0] }
        /*, input_parameters_{ argv + 1, argv + argc }*/
    {
        std::vector<std::string> fix_input{ argv + 1, argv + argc };
        // merge parameters to accept "-input.name:value" parameter name, dot is splitting params
        for (int i = 0; i < fix_input.size(); ++i)
        {
            if (i + 1 < fix_input.size() && fix_input[i + 1][0] == '.' && fix_input[i][0] == '-')
            {
                input_parameters_.emplace_back(fix_input[i] + fix_input[i + 1]);
                ++i;
            }
            else input_parameters_.emplace_back(std::move(fix_input[i]));
        }
    }

    void core::process()
    {
        for (const auto& module : modules_)
        {
            const auto& commands_view = module->commands().get();
            bool match = search(module->commands().get());

            if (!match)
            {
                // execute the root command (ncs_module command)
                if (!commands_view.empty() && commands_view[0]->name() == "ncs_module")
                {
                    ncs::input_command input_command;
                    ncs::commands<>::parse_parameters(input_parameters_, input_command, 2); // todo don't hardcode parameter index
                    ncs::command_executor executor{ *(commands_view[0]), input_command };
                    executor.process();
                }
            }
        }

        //if (!match) suggest(argc, argv);

    }

    bool core::search(const ncs::commands_view& commands)
    {
        for (const auto& command : commands)
        {
            //std::cout << "\nCommand: " << command->str_path();
            int node_level = 0;

            for (const auto& element : input_parameters_)
            {
                //std::cout << "\n- Testing: " << command->path().node(node_level) << " | " << element;
                if (command->path().node(node_level) == element)
                {
                    //std::cout << " found";
                    // command path match, check command name
                    if (command->path().size() == node_level + 1
                        && command->name() == element)
                    {
                        //std::cout << " | Match ";

                        ncs::input_command input_command;
                        ncs::commands<>::parse_parameters(input_parameters_, input_command, node_level + 1);
                        ncs::command_executor executor{ *command, input_command };
                        executor.process();
                        return true;
                    }
                    ++node_level;
                }
                else break;
            }
        }
        return false;
    }

} // ncs