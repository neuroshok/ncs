#ifndef INCLUDE_NCS_COMMANDS_HPP_NCS
#define INCLUDE_NCS_COMMANDS_HPP_NCS

#include <ncs/command.hpp>
#include <ncs/command/executor.hpp>
#include <ncs/type.hpp>
#include <color.hpp>

#include <memory>
#include <ranges>

#include <string>
#include <string_view>

namespace ncs
{
    class cmd;

    class commands_base
    {
    public:
        virtual ncs::command_ptr add(ncs::command command) = 0;
        virtual const ncs::commands_view& get() const = 0;
        virtual ~commands_base() = default;
    };

    //! set of commands, T: crtp to extend command node graph
    //! ncs::commands<extend>, node<commands>, access by cmds.extend
    template<class T = ncs::cmd>
    class commands : public commands_base
    {
    public:
        static constexpr const char* optional_prefix = "-";
        static constexpr const char* value_prefix = ":";
        static constexpr const char* os_separator = "\\";
        static constexpr const char* os_extension = ".";

        explicit commands(std::string command_path)
            : module_name_{ path_to_module(std::move(command_path)) }
        {}

        /*
        void process(std::string_view input)
        {
            input_parameters.clear();
            //todo fix ranges
            for (auto ranges : std::views::split(input, ' '))
            {
                std::string word;
                for (auto c : ranges) word += c;
                auto view =  std::string_view{ word } | std::views::drop_while(isspace) | std::views::reverse | std::views::drop_while(isspace) | std::views::reverse;
                word = std::string{ view.begin(), view.end() };
                if (!word.empty()) input_parameters.push_back(std::move(word));
            }

            process();
        }*/


        void suggest()
        {

        }

        ncs::command_ptr add(ncs::command command) override
        {
            commands_.emplace_back(std::make_unique<ncs::command>(std::move(command)));
            return commands_.back().get();
        }

        template<class Fn, class... Ts>
        ncs::command_ptr add(ncs::path path, Fn&& fn, std::string description, Ts&&... ts)
        {
            commands_.emplace_back(std::move(path), std::forward<Fn>(fn), std::move(description), std::forward<Ts>(ts)...);
            return commands_.back().get();
        }

        template<class Fn, class... Ts>
        ncs::command_ptr add(const char* command_name, Fn&& fn, std::string description, Ts&&... ts)
        {
            commands_.emplace_back(ncs::path{ module_name_, std::string(command_name) }
            , std::forward<Fn>(fn), std::move(description), std::forward<Ts>(ts)...);
            return commands_.back().get();
        }

        void error()
        {
            std::cout << "no command found, use " + module_name_ + " help\n";
        }

        [[nodiscard]] const ncs::commands_view& get() const override { return commands_; }
        [[nodiscard]] const std::string& module_name() const { return module_name_; }

        void help()
        {
            std::cout << dye::green("usage") << ": " + module_name_ + " (command_node...) command \n";
            std::cout << "        (value... | -name(.field...)(:value)...) \n";
            for (const auto& command : commands_)
            {
                if (command->name() == "ncs_module") std::cout << "\n" <<  dye::aqua(module_name_);
                else std::cout << "\n" << dye::aqua(command->str_path());


                if (!command->description().empty()) std::cout << " --  " << command->description();

                for (const auto& parameter : command->parameters())
                {
                    std::string parameter_name = parameter.name();
                    if (parameter.required()) parameter_name = "<" + parameter.name() + ">";
                    else parameter_name = "-" + parameter.name() + ":" + ncs::parameter::to_string(parameter.default_value());

                    std::cout << "\n    " << dye::yellow(parameter_name);
                    if (!parameter.description().empty()) std::cout << " -- " << parameter.description();
                }
                //std::cout << "\n";
            }
            std::cout << "\n";
        }

        static std::string path_to_module(std::string path)
        {
            // get program name
            auto separator_index = path.find_last_of(os_separator);
            if (separator_index != std::string::npos)
            {
                path = path.substr(separator_index + 1);
                auto extension_index = path.find_last_of(os_extension);
                if (extension_index != std::string::npos) path = path.substr(0, extension_index);
            }
            return path;
        }


        static void parse_parameters(const std::vector<std::string>& input_parameters, ncs::input_command& input_command, int parameters_index = 0)
        {
            for (std::size_t i = parameters_index; i < input_parameters.size(); ++i)
            {
                const auto& param_expression = input_parameters[i];

                // parse optional param
                if (param_expression.substr(0, 1) == optional_prefix)
                {
                    // parse param name
                    std::string param_name;
                    bool has_value = false;
                    for (std::size_t j = 0; j < param_expression.size(); ++j)
                    {
                        // param has value
                        if (param_expression.substr(j, 1) == value_prefix)
                        {
                            has_value = true;
                            param_name = param_expression.substr(1, j - 1);

                            // parse param value
                            input_command.add_parameter(param_name, param_expression.substr(j + 1));
                            break;
                        }
                    }
                    if (!has_value)
                    {
                        // without value, interpret parameter as a flag
                        input_command.add_flag_parameter(param_expression.substr(1));
                    }
                }
                // add positional param value
                else input_command.add_value(param_expression);
            }
        }

    private:
        std::string module_name_;
        ncs::commands_view commands_;
    };
} // ncs

#endif // INCLUDE_NCS_COMMANDS_HPP_NCS