#ifndef INCLUDE_NCS_TYPE_HPP_NCS
#define INCLUDE_NCS_TYPE_HPP_NCS

#include <vector>
#include <memory>

namespace ncs
{
    class command;

    using commands_view = std::vector<std::unique_ptr<ncs::command>>;

    struct required_{};
    static constexpr required_ required;
};

#endif // INCLUDE_NCS_TYPE_HPP_NCS