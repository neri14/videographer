#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include <string>
#include <format>
#include <vector>
#include <map>
#include <optional>

#include "logging/logger.h"

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
    argument& description(const std::string& desc);

    std::vector<std::string> options_ = {};
    bool is_flag_ = false;
    std::string description_ = "";
};

class argument_parser {
public:
    argument_parser(const std::string binary_name);
    ~argument_parser() = default;
    argument_parser(const argument_parser&) = default;
    argument_parser(argument_parser&&) = default;
    argument_parser& operator=(const argument_parser&) = default;
    argument_parser& operator=(argument_parser&&) = default;

    void add_argument(const std::string& key, const argument& arg);

    void parse(int argc, char* argv[]);

    bool has(const std::string& key) const;
    int count(const std::string& key) const;

    template <typename T>
    T get(const std::string& key) const
    {
        throw argument_exception(std::format("Unsupported argument type requested for {} argument", key));
    }

    std::map<std::string, std::vector<std::string>> get_raw() const;

    void print_help() const;
    void print_version() const;

private:
    void assert_key_exists(const std::string& key) const;
    void assert_key_provided(const std::string& key) const;
    void assert_at_least_one_value(const std::string& key) const;
    void assert_at_most_one_value(const std::string& key) const;

    void print_help_usage() const;
    void print_help_details() const;

    const std::string binary_name_;

    std::vector<std::string> keys_; // vector of all supported keys in order of addition
    std::map<std::string, argument> arguments_; // mapping argument keys to argument objects
    std::map<std::string, std::string> options_; // mapping options to argument keys

    std::map<std::string, std::vector<std::string>> values_; // parsed values of parsed keys mapped to optional raw value
};

} // namespace utils
} // namespace vgraph

#endif // ARGUMENT_PARSER_H
