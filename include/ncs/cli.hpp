#ifndef INCLUDE_NCS_CLI_HPP_NCS
#define INCLUDE_NCS_CLI_HPP_NCS

#include <ncs/command.hpp>
#include <ncs/command/executor.hpp>
#include <color.hpp>

#include <memory>
#include <ranges>
#include <string>
#include <string_view>

namespace ncs
{
    class cli_base
    {
    public:
        virtual command_ptr add(ncs::command command) = 0;
        virtual ~cli_base() = default;
    };

    template<class T>
    class basic_cli : public cli_base
    {
    public:
        static constexpr const char* optional_prefix = "-";
        static constexpr const char* value_prefix = ":";
        static constexpr const char* os_separator = "\\";
        static constexpr const char* os_extension = ".";

        explicit basic_cli(std::string command_path)
            : module_name_{ path_to_module(std::move(command_path)) }
            , params_index_{}
        {}

        void process()
        {
            params_index_ = 0;

            // search command
            bool match = search();
            //if (!match) suggest(argc, argv);
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

        void process(std::string_view input)
        {
            input_.clear();
            //todo fix ranges
            for (auto ranges : std::views::split(input, ' '))
            {
                std::string word;
                for (auto c : ranges) word += c;
                auto view =  std::string_view{ word } | std::views::drop_while(isspace) | std::views::reverse | std::views::drop_while(isspace) | std::views::reverse;
                word = std::string{ view.begin(), view.end() };
                if (!word.empty()) input_.push_back(std::move(word));
            }

            process();
        }

        void process(int argc, const char* argv[])
        {
            input_.clear();
            input_.reserve(argc);
            for (int i = 0; i < argc; ++i)
            {
                input_.emplace_back(argv[i]);
            }
            input_[0] = module_name_;

            process();
        }

        bool search()
        {
            for (const auto& command : commands_)
            {
                unsigned int node_level = 0;

                for (const auto& element : input_)
                {
                    if (command->path().node(node_level) == element)
                    {
                        // command path match, check command name
                        if (command->path().size() == node_level + 1
                            && command->name() == element)
                        {
                            params_index_ = node_level + 1;

                            ncs::input_command input_command;
                            parse_params(input_command);
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

        void suggest()
        {

        }

        void parse_params(ncs::input_command& input_command)
        {
            for (std::size_t i = params_index_; i < input_.size(); ++i)
            {
                const auto& param_expression = input_[i];

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

        [[nodiscard]] const std::string& module_name() const { return module_name_; }
        T& get() { return static_cast<T&>(*this); }

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

    private:
        unsigned int params_index_;
        std::vector<std::string> input_;
        std::string module_name_;
        std::vector<std::unique_ptr<ncs::command>> commands_;
    };
} // ncs

#endif // INCLUDE_NCS_CLI_HPP_NCS