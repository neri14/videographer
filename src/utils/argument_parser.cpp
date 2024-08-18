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
    argtype_ = EArgType::Bool;
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

argument& argument::argtype(EArgType t)
{
    if (is_flag_ && EArgType::Bool != t)
        throw argument_exception("Setting flag type to non bool");

    argtype_ = t;
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

void argument_parser::print_help() const
{
    std::string opts;
    std::vector<std::string> details;

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

        // std::string det = std::format("  {:<16}   {}", )
    }

    std::cout << std::format("Usage: {} {}", binary_name_, opts) << std::endl;
    std::cout << "Arguments:" << std::endl;

    //TODO printing details as below example
}

// void args::print_help()
// {
//     std::cout << "Usage: vgraph [-h][-d]" << std::endl;
//     std::cout << "Arguments:" << std::endl;
//     std::cout << "  -h, --help    Print this help message" << std::endl;
//     std::cout << "  -d, --debug   Enable debug logs" << std::endl;
// }

} // namespace utils
} // namespace vgraph
