#ifndef MODULE_NCS_MODULE_PROJECT_COMMANDS_IMPL_HPP_NCS
#define MODULE_NCS_MODULE_PROJECT_COMMANDS_IMPL_HPP_NCS

namespace ncs { class parameters; }
namespace ncs::modules { class project; }

// ncs_module_namespace(project) {}
namespace ncs::internal::modules::project
{
    struct commands_impl
    {
        explicit commands_impl(ncs::modules::project& project) : project_{ project } {}

        void make(const ncs::parameters& input);

    private:
        ncs::modules::project& project_;
    };

} // ncs

#endif // MODULE_NCS_MODULE_PROJECT_COMMANDS_IMPL_HPP_NCS