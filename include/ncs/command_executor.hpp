#ifndef INCLUDE_NCS_COMMAND_EXECUTOR_HPP_NCS
#define INCLUDE_NCS_COMMAND_EXECUTOR_HPP_NCS

#include <ncs/command.hpp>
#include <ncs/input.hpp>

namespace ncs
{
    class command_executor
    {
    public:
        command_executor(const ncs::command& command, const ncs::input_command& input)
            : command_{ command }
            , input_{ input }
        {}

        bool is_valid() const
        {
            for (const auto& parameter : command_.parameters())
            {
                std::cout << "\ncheck " << parameter.name() << " " << parameter.required();
                input.
            }
            return true;
        }

        void process()
        {
            if (!is_valid())
            {
                std::cout << "\ninvalid input";
                return;
            }
            command_.exec(input_);
        }

    private:
        const ncs::command& command_;
        const ncs::input_command& input_;
    };
} // ncs

#endif // INCLUDE_NCS_COMMAND_EXECUTOR_HPP_NCS