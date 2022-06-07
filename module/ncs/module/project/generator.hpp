#ifndef MODULE_NCS_MODULE_PROJECT_GENERATOR_HPP_NCS
#define MODULE_NCS_MODULE_PROJECT_GENERATOR_HPP_NCS

#include <string>

namespace ncs
{
    class core;
    class parameters;
} // ncs

namespace ncs::internal::modules::project
{
    class generator
    {
    public:
        explicit generator(ncs::core& core, std::string meta_prefix);

        void process(const ncs::parameters& parameters);

    protected:
        std::string evaluate_name(const std::string&);

    private:
        ncs::core& core_;

        std::string project_name_;
        std::string meta_prefix_;
        inline static const std::string_view var_project_name = "project.name";
    };
} // ncs

#endif // MODULE_NCS_MODULE_PROJECT_GENERATOR_HPP_NCS