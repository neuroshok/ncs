#ifndef MODULE_PROJECT_PROJECT_HPP_NCS
#define MODULE_PROJECT_PROJECT_HPP_NCS

/*
// path relative to entity root
ncs project make -path:ark/cpp -license:mit -name:ncs -vcs:git -ncs.default<path license vcs> // default parameters
ncs project make -name:test // use default parameters, name required

//
ncs context project xmake
ncs make -name:test
ncs config --qt:d:/qt // xmake command
ncs build // cmake build

//
ncs project make -path:ark/cpp -license:mit -name:ncs -vcs:git
ncs edge @ark https://www.github.com/arkena00
ncs edge @jo https://www.github.com/jo
ncs project make @jo:cpp

// ncs status or ncs info

 ncs project make -tpl:ark/cpp -name:test
*/

#include <ncs/module.hpp>
#include <ncs/parameter.hpp>
#include <ncs/module/project/generator.hpp>
#include <ncs/module/project/commands.hpp>
#include <ncs/module/project/commands_impl.hpp>

namespace ncs::modules
{
    namespace mod = ncs::internal::modules::project;

    class project : public ncs::module
    {
    public:
        explicit project(ncs::core& core)
            : ncs::module(core, "project")
            , core_{ core }
            , commands_impl_{ *this }
            , project_commands_{ commands_impl_ }
            , commands_node_{ project_commands_ }
        {

            //core_.add_commands(commands_);
        }

        /*
        void make(const ncs::parameters& input)
        {
            mod::generator generator{ core_, "$ncs" };
            generator.process(input);
        }*/

        ncs::core& core() { return core_; }

        [[nodiscard]] ncs::commands_base& commands() override { return commands_node_.get_commands(); }

    private:
        ncs::core& core_;
        mod::project_commands project_commands_;
        mod::commands_node commands_node_;
        mod::commands_impl commands_impl_;
    };
} // ncs

#endif // MODULE_PROJECT_PROJECT_HPP_NCS