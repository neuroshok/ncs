#ifndef MODULE_NCS_MODULE_PROJECT_GENERATOR_HPP_NCS
#define MODULE_NCS_MODULE_PROJECT_GENERATOR_HPP_NCS

#include "ncs/parameters.hpp"
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
        std::string get_input_data(const std::string&);
        void load_commands();
        void load_variables();
        std::string process_file(const std::filesystem::path& source_origin, const std::filesystem::path& file);
        std::string evaluate_name(const std::string&);
        void run(const std::string& command, const std::filesystem::path& execution_path = std::filesystem::current_path());

    private:
        ncs::core& core_;
        bool clean_source_ = false;

        std::filesystem::path source_origin_;
        std::filesystem::path target_origin_;
        std::vector<std::string> commands_;
        std::vector<std::pair<std::string, std::string>> variables_;
        ncs::parameters dynamic_inputs_;

        std::string project_name_;
        std::string meta_prefix_;
    };
} // ncs

#endif // MODULE_NCS_MODULE_PROJECT_GENERATOR_HPP_NCS