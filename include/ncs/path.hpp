#ifndef INCLUDE_NCS_PATH_HPP_NCS
#define INCLUDE_NCS_PATH_HPP_NCS

#include <string>
#include <vector>

namespace ncs
{
    class path
    {
        using node_type = std::string;
        
    public:
        template<class... Ts>
        explicit path(Ts... ts)
            : str_path_{ (std::string{} + ... + (ts + std::string(" "))) }
            , nodes_{ std::move(ts)... }
        {}

        path(ncs::path path, std::string node)
            : str_path_{ std::move(path.str_path_) }
            , nodes_{ std::move(path.nodes_) }
        {
            add(std::move(node));
        }

        void add(std::string node_name)
        {
            str_path_ = str_path_ + node_name + " ";
            nodes_.emplace_back(std::move(node_name));
        }
        [[nodiscard]] const node_type& node(unsigned int n) const { return nodes_[n]; }
        [[nodiscard]] const node_type& last() const { return nodes_.back(); }
        [[nodiscard]] const std::string& str() const { return str_path_; }
        [[nodiscard]] std::size_t size() const { return nodes_.size(); }
        [[nodiscard]] const std::vector<node_type>& data() const { return nodes_; }
        
    private:
        std::string str_path_;
        std::vector<node_type> nodes_;
    };
} // ncs

#endif // INCLUDE_NCS_PATH_HPP_NCS