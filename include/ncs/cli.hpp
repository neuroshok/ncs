#ifndef INCLUDE_NCS_CLI_HPP_NCS
#define INCLUDE_NCS_CLI_HPP_NCS

#include <ncs/command.hpp>

#include <iostream>
#include <string>

namespace ncs
{
    class cli_base
    {
    public:
        virtual void add(ncs::command command) = 0;
    };

    template<class T>
    class cli : public cli_base
    {
    public:
        cli(std::string command_root)
            : module_name_{ std::move(command_root) }
        {}

        void process(int argc, const char* argv[])
        {
            for (int i = 0; i < argc; ++i)
            {
                std::cout << "\n" << argv[i];
            }
        }
        
        void add(ncs::command command) override
        {
            commands_.emplace_back(std::move(command));
        }

        template<class Fn, class... Ts>
        void add(ncs::path path, Fn&& fn, std::string description, Ts&&... ts)
        {
            commands_.emplace_back(std::move(path), std::forward<Fn>(fn), std::move(description), std::forward<Ts>(ts)...);
        }

        template<class Fn, class... Ts>
        void add(const char* command_name, Fn&& fn, std::string description, Ts&&... ts)
        {
            commands_.emplace_back(ncs::path{ module_name_, std::string(command_name) }
            , std::forward<Fn>(fn), std::move(description), std::forward<Ts>(ts)...);
        }

        const std::string& module_name() const { return module_name_; }
        T& get() { return static_cast<T&>(*this); }

        void help()
        {
            std::cout << "usage : " + module_name_ + " (command_node...) command \n";
            std::cout << "        (value... | -name(.field...)(:value)...) \n\n";
            std::cout << "*************************************\ncommands : \n";
            for (const auto& command : commands_)
            {
                std::cout << "\n" << command.str_path();
                if (!command.description().empty()) std::cout << "\n\t" << command.description();

                for (const auto& parameter : command.parameters())
                {
                    std::string parameter_name = parameter.name();
                    if (parameter.required()) parameter_name = "<" + parameter.name() + ">";
                    else parameter_name = "-" + parameter.name() + ":" + ncs::parameter::to_string(parameter.default_value());

                    std::cout << "\n\t" << parameter_name << " " << parameter.description();
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        }

    private:
        std::string module_name_;
        std::vector<ncs::command> commands_;
    };
} // ncs

#endif // INCLUDE_NCS_CLI_HPP_NCS