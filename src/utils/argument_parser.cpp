#include "argument_parser.h"

#include <format>
#include <iostream>
#include <numeric>
#include <algorithm>

namespace vgraph {
namespace utils {
namespace helper {

std::string to_upper(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::toupper(c); });
    return std::move(str);
}

} // namespace helper

argument_exception::argument_exception(
    const std::string& msg): msg_(std::format("Argument exception: {}", msg)) {}

const char* argument_exception::what() const noexcept
{
    return msg_.c_str();
}

argument& argument::option(const std::string& opt)
{
    if (opt.rfind('-', 0) != 0) {
        throw argument_exception("Option does not start with a dash");
    }
    if (opt.find(' ') != std::string::npos) {
        throw argument_exception("Option contains spaces");
    }
    if (std::find(options_.begin(), options_.end(), opt) != options_.end()) {
        throw argument_exception("Duplicate option in argument definition");
    }

    options_.push_back(opt);
    return *this;
}

argument& argument::flag()
{
    is_flag_ = true;
    return *this;
}

argument& argument::description(const std::string& desc)
{
    description_ = desc;
    return *this;
};

argument_parser::argument_parser(const std::string binary_name) :
    binary_name_(binary_name)
{}

void argument_parser::add_argument(const std::string& key, const argument& arg)
{
    if (arg.options_.empty())
        throw argument_exception("No option defined for argument");

    if (arguments_.contains(key))
        throw argument_exception("Duplicate argument key");

    for (const std::string& opt : arg.options_)
        if (options_.contains(opt))
            throw argument_exception("Argument with already defined option added");

    keys_.push_back(key);
    arguments_[key] = arg;

    for (const std::string& opt : arg.options_)
        options_[opt] = key;
}

void argument_parser::parse(int argc, char* argv[])
{
    for (int i=1; i<argc; i++) {
        std::string opt(argv[i]);

        if (!options_.contains(opt))
            throw argument_exception(std::format("Undefined option {}", opt));

        const std::string& key = options_.at(opt);
        const argument& arg = arguments_.at(key);

        if (arg.is_flag_) {
            values_[key] = std::vector<std::string>();
            continue;
        }

        if (++i >= argc)
            throw argument_exception(std::format("Missing value for {}", opt));

        std::string val(argv[i]);

        if (!values_.contains(key)) {
            values_[key] = std::vector<std::string>();
        }
        values_[key].push_back(val);
    }
}

bool argument_parser::has(const std::string& key) const
{
    if (!arguments_.contains(key))
        throw argument_exception("Undefined argument key retrieval attempt");

    return values_.contains(key);
}

int argument_parser::count(const std::string& key) const
{
    if (!has(key))
        return 0;

    return values_.at(key).size();
}

template <>
std::string argument_parser::get(const std::string& key) const
{
    if (!arguments_.contains(key))
        throw argument_exception(std::format("Undefined argument \"{}\" retrieval attempt", key));

    if (!has(key))
        throw argument_exception(std::format("Retrieval of not provided argument \"{}\" value", key));

    const auto& val = values_.at(key);
    if (val.size() < 1)
        throw argument_exception(std::format("Retrieval of argument \"{}\" value that has no associated value", key));
    if (val.size() > 1)
        throw argument_exception(std::format("Retrieval of singular argument \"{}\" value that has more values", key));

    return val[0];
}

template <>
int argument_parser::get(const std::string& key) const
{
    if (!arguments_.contains(key))
        throw argument_exception(std::format("Undefined argument \"{}\" retrieval attempt", key));

    if (!has(key))
        throw argument_exception(std::format("Retrieval of not provided argument \"{}\" value", key));

    const auto& val = values_.at(key);
    if (val.size() < 1)
        throw argument_exception(std::format("Retrieval of argument \"{}\" value that has no associated value", key));
    if (val.size() > 1)
        throw argument_exception(std::format("Retrieval of singular argument \"{}\" value that has more values", key));

    int ret = 0;
    try {
        ret = std::stoi(val[0]);
    } catch(std::invalid_argument) {
        throw argument_exception(std::format("Error parsing value \"{}\" of argument \"{}\" as int", val[0], key));
    } catch(std::out_of_range) {
        throw argument_exception(std::format("Value \"{}\" of argument \"{}\" is out of range", val[0], key));
    }
    return ret;
}//FIXME to be refactored after merging with improved arguments parser

template <>
std::vector<std::string> argument_parser::get(const std::string& key) const
{
    if (!arguments_.contains(key))
        throw argument_exception(std::format("Undefined argument \"{}\" retrieval attempt", key));

    if (!has(key))
        throw argument_exception(std::format("Retrieval of not provided argument \"{}\" value", key));

    const auto& val = values_.at(key);
    if (val.size() < 1)
        throw argument_exception(std::format("Retrieval of argument \"{}\" value that has no associated value", key));

    return val;
}

std::map<std::string, std::vector<std::string>> argument_parser::get_raw() const
{
    return values_;
}

template <>
bool argument_parser::get(const std::string& key) const
{
    return has(key);
}

void argument_parser::print_help() const
{
    print_help_usage();
    print_help_details();
}

void argument_parser::print_help_usage() const
{
    std::string opts;

    for (const std::string& key : keys_) {
        const auto& arg = arguments_.at(key);

        std::string str = *std::min_element(
            arg.options_.begin(),
            arg.options_.end(),
            [](const std::string& a, const std::string& b){
                return a.length() < b.length();
            });

        if (!arg.is_flag_) {
            str = std::format("{} {}", str, helper::to_upper(key));
        }

        opts += std::format("[{}]", str);
    }

    std::cout << std::format("Usage: {} {}", binary_name_, opts) << std::endl;
}

void argument_parser::print_help_details() const
{
    std::cout << "Arguments:" << std::endl;


    for (const std::string& key : keys_) {
        const auto& arg = arguments_.at(key);

        std::string str = std::accumulate(
            arg.options_.begin(),
            arg.options_.end(),
            std::string{},
            [](const std::string& prev, const std::string& add){
                return prev.empty() ? add : std::format("{},{}", prev, add);
            });

        if (!arg.is_flag_) {
            str = std::format("{} {}", str, helper::to_upper(key));
        }

        if (str.length() > 20) {
            std::cout << std::format("  {}", str) << std::endl;
            std::cout << std::format("                    {}", arg.description_) << std::endl;
        } else {
            std::cout << std::format("  {:<16}  {}", str, arg.description_) << std::endl;
        }
    }
}

} // namespace utils
} // namespace vgraph
