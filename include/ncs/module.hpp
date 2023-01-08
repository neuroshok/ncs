#ifndef INCLUDE_NCS_MODULE_HPP_NCS
#define INCLUDE_NCS_MODULE_HPP_NCS

#include <string>

namespace ncs
{
    class core;
    class commands_base;

    class module
    {
    public:
        module(ncs::core& core, std::string name)
            : core_{ core }
            , name_{ std::move(name) }
        {}

        [[nodiscard]] const std::string& name() const { return name_; }
        [[nodiscard]] virtual ncs::commands_base& commands() = 0;

    protected:
        [[nodiscard]] ncs::core& core() { return core_; }

    private:
        ncs::core& core_;
        std::string name_;
    };
}

#endif // INCLUDE_NCS_MODULE_HPP_NCS