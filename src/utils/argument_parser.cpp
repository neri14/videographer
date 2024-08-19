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
    const std::string& msg): msg_(msg) {}

const char* argument_exception::what() const noexcept
{
    return std::format("Argument exception: {}", msg_).c_str();
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
    if (is_mandatory_)
        throw argument_exception("Mandatory argument set as flag");

    is_flag_ = true;
    return *this;
}

argument& argument::mandatory()
{
    if (is_flag_)
        throw argument_exception("Flag set as mandatory");

    is_mandatory_ = true;
    return *this;
};

argument& argument::max_count(int max) 
{
    max_count_ = max;
    return *this;
};

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
    //TODO to be implemented: void argument_parser::parse(int argc, char* argv[])
}

bool argument_parser::has(const std::string& key) const
{
    return values_.contains(key);
}

template <>
std::string argument_parser::get(const std::string& key) const
{
    if (!arguments_.contains(key))
        throw argument_exception("Undefined argument key retrieval attempt");

    if (!has(key))
        throw argument_exception("Retrieval of not provided argument value");

    const auto& val = values_.at(key);
    if (val)
        return *val;
    
    throw argument_exception("Argument has no raw string value");
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

        if (arg.is_mandatory_) {
            opts += std::format(" {} ", str);
        } else {
            opts += std::format("[{}]", str);
        }
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
