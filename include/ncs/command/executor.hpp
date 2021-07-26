#ifndef INCLUDE_NCS_COMMAND_EXECUTOR_HPP_NCS
#define INCLUDE_NCS_COMMAND_EXECUTOR_HPP_NCS

#include <ncs/command.hpp>
#include <ncs/input/command.hpp>

#include <iostream>

namespace ncs
{
    class command_executor
    {
    public:
        command_executor(const ncs::command& command, const ncs::input_command& input)
            : command_{ command }
            , input_{ input }
        {}

        void process()
        {
            ncs::parameters params;

            bool valid = true;
            int index = 0;

            for (const auto& parameter : command_.parameters())
            {
                if (!input_.parameter_exist(parameter.name()))
                {
                    // search for index parameter
                    if (input_.parameter_exist(index))
                    {
                        bool add_ok = params.add(parameter, input_.parameter(index).value);
                        if (!add_ok)
                        {
                            valid = false;
                            errors_ += "- Error while processing value: " + input_.parameter(index).value;
                        }
                        ++index;
                        continue;
                    }

                    if (!parameter.has_default_value())
                    {
                        errors_ += "\n- Missing parameter <" + parameter.name() + ">";
                        valid = false;
                    }
                    else params.add_default(parameter, false);
                }
                else
                {
                    auto input_parameter = input_.parameter(parameter.name());
                    if (input_parameter.is_flag)
                    {
                        if (parameter.required())
                        {
                            errors_ += "\n- Missing value for <" + parameter.name() + ">";
                            valid = false;
                        }
                        else params.add_default(parameter);
                    }
                    else
                    {
                        bool add_ok = params.add(parameter, input_parameter.value);
                        if (!add_ok)
                        {
                            valid = false;
                            errors_ += "- Error while processing value: " + input_parameter.value;
                        }
                    }
                }
            }

            if (!valid)
            {
                std::cout << "Command error ";
                std::cout << errors_;
                return;
            }

            command_.exec(params);
        }

    private:
        const ncs::command& command_;
        const ncs::input_command& input_;
        std::string errors_;
    };
} // ncs

#endif // INCLUDE_NCS_COMMAND_EXECUTOR_HPP_NCS