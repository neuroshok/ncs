#ifndef MODULE_NCS_MODULE_PROJECT_GENERATOR_HPP_NCS
#define MODULE_NCS_MODULE_PROJECT_GENERATOR_HPP_NCS

#include <string>
#include <filesystem>

namespace ncs
{
    class core;
    class parameters;
} // ncs

namespace ncs::modules { class project; }

namespace ncs::internal::modules::project
{
    class generator
    {
    public:
        explicit generator(ncs::modules::project& core, std::string meta_prefix);

        void process(const ncs::parameters& parameters);

    protected:
        void load_commands();
        void load_variables();
        std::string process_file(const std::filesystem::path& source_origin, const std::filesystem::path& file);
        std::string evaluate_name(const std::string&);

    private:
        ncs::core& core_;

        std::filesystem::path source_origin_;
        std::vector<std::string> commands_;
        std::vector<std::pair<std::string, std::string>> variables_;

        std::string project_name_;
        std::string meta_prefix_;
    };
} // ncs

#endif // MODULE_NCS_MODULE_PROJECT_GENERATOR_HPP_NCS