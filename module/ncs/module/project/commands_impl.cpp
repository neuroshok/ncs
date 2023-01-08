#include <ncs/module/project/commands_impl.hpp>
#include <ncs/module/project/generator.hpp>

namespace ncs::modules { class project; }

// ncs_module_namespace(project) {}
namespace ncs::internal::modules::project
{
    void commands_impl::make(const ncs::parameters& input)
    {
        ncs::internal::modules::project::generator generator{ project_, "$ncs" };
        generator.process(input);
    }
}