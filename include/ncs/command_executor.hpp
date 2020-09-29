#ifndef INCLUDE_NCS_COMMAND_EXECUTOR_HPP_NCS
#define INCLUDE_NCS_COMMAND_EXECUTOR_HPP_NCS

#include <ncs/command.hpp>
#include <ncs/input.hpp>

namespace ncs
{
    class command;
    
    class command_executor
    {
    public:
        command_executor(const ncs::command& command, const ncs::input_command& input)
            : command_{ command }
            , input_{ input }
        {}

        void process()
        {
            command_.exec();
        }

    private:
        const ncs::command& command_;
        const ncs::input_command& input_;
    };
} // ncs

#endif // INCLUDE_NCS_COMMAND_EXECUTOR_HPP_NCS