#include <gtest/gtest.h>
#include <cstring>

#include "utils/args.h"

namespace vgraph {
namespace utils {

class args_base_test {
protected:
    args call_parse(std::vector<std::string> vargs)
    {
        int argc = vargs.size() + 1;
        char* argv[argc];

        argv[0] = new char[6];
        argv[0] = std::strcpy(argv[0], "dummy");

        int i = 1;
        for (const std::string& arg : vargs) {
            argv[i] = new char[arg.size() + 1];
            std::strcpy(argv[i], arg.c_str());
            i++;
        }

        args a = args::parse(argc, argv);

        for (int i = 0; i < argc; i++) {
            delete[] argv[i];
        }

        return a;
    }
};

class args_test: public args_base_test, public ::testing::Test {};

TEST_F(args_test, default_args)
{
    args a = call_parse({});

    EXPECT_EQ(false, a.help);
    EXPECT_EQ(false, a.debug);
}


class args_flag_test: public args_base_test, public ::testing::TestWithParam<std::tuple<const char*, bool args::*>> {};

TEST_P(args_flag_test, boolean_flag_present)
{
    auto [flag, field] = GetParam();

    args a = call_parse({flag});
    EXPECT_EQ(true, a.*field);
}

TEST_P(args_flag_test, boolean_flag_missing)
{
    auto [flag, field] = GetParam();

    args a = call_parse({});
    EXPECT_EQ(false, a.*field);
}

INSTANTIATE_TEST_SUITE_P(
    ,
    args_flag_test,
    ::testing::Values(
        std::make_tuple("-h",       &args::help),
        std::make_tuple("--help",   &args::help),
        std::make_tuple("-d",       &args::debug),
        std::make_tuple("--debug",  &args::debug)
    )
);

} // namespace utils
} // namespace vgraph
