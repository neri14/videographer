#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include <string>
#include <vector>
#include <map>

namespace vgraph {
namespace utils {

class argument_exception : public std::exception {
public:
    argument_exception(const std::string& msg);
    const char* what() const noexcept override;

private:
    std::string msg_;
};

enum class EArgType {
    String,
    Int,
    Bool
};

struct argument {
    argument& option(const std::string& opt);
    argument& flag();
    argument& mandatory();
    argument& max_count(int max);
    argument& argtype(EArgType t);
    argument& description(const std::string& desc);

    std::vector<std::string> options_ = {};
    bool is_flag_ = false;
    bool is_mandatory_ = false;
    int max_count_ = 1;
    EArgType argtype_ = EArgType::String;
    std::string description_ = "";
};

// class value {
// public:
//     value(const std::string& raw);
//     value(bool val);

//     bool get_bool();
//     std::string get_string();
//     int get_integer();

// private:
//     std::string raw_value;
//     bool bool_value;
// };



class argument_parser {
public:
    argument_parser(const std::string binary_name);
    ~argument_parser() = default;
    argument_parser(const argument_parser&) = delete;

    void add_argument(const std::string& key, const argument& arg);

    //TODO std::map<std::string, value> parse(int argc, char* argv[]);
    void print_help() const;

private:
    const std::string binary_name_;

    std::vector<std::string> keys_;
    std::map<std::string, argument> arguments_;
    std::map<std::string, std::string> options_;
};

} // namespace utils
} // namespace vgraph

#endif // ARGUMENT_PARSER_H
