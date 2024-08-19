#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <optional>

namespace vgraph {
namespace utils {

class argument_exception : public std::exception {
public:
    argument_exception(const std::string& msg);
    const char* what() const noexcept override;

private:
    std::string msg_;
};

struct argument {
    argument& option(const std::string& opt);
    argument& flag();
    argument& mandatory();
    argument& max_count(int max);
    argument& description(const std::string& desc);

    std::vector<std::string> options_ = {};
    bool is_flag_ = false;
    bool is_mandatory_ = false;
    int max_count_ = 1;
    std::string description_ = "";
};

class argument_parser {
public:
    argument_parser(const std::string binary_name);
    ~argument_parser() = default;
    argument_parser(const argument_parser&) = delete;

    void add_argument(const std::string& key, const argument& arg);

    void parse(int argc, char* argv[]);

    bool has(const std::string& key) const;

    template <typename T>
    T get(const std::string& key) const
    {
        throw argument_exception("Unsupported argument type requested for {} argument", key);
    }

    void print_help() const;

private:
    void print_help_usage() const;
    void print_help_details() const;

    const std::string binary_name_;

    std::vector<std::string> keys_;
    std::map<std::string, argument> arguments_;
    std::map<std::string, std::string> options_;

    std::map<std::string, std::optional<std::string>> values_;
};

} // namespace utils
} // namespace vgraph

#endif // ARGUMENT_PARSER_H
