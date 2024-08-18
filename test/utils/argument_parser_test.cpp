#include <gtest/gtest.h>
#include <cstring>

#include "utils/argument_parser.h"

namespace vgraph {
namespace utils {
namespace consts {
const std::string BIN_NAME("binary");
}

class argument_parser_test: public ::testing::Test{
};


TEST_F(argument_parser_test, flag_type_is_bool)
{
    EXPECT_EQ(EArgType::Bool, argument().flag().argtype_);
    EXPECT_NO_THROW(argument().flag().argtype(EArgType::Bool));
    EXPECT_NO_THROW(argument().argtype(EArgType::Bool).flag());
}

TEST_F(argument_parser_test, creating_mandatory_flag_throws)
{
    EXPECT_THROW(argument().flag().mandatory(), argument_exception);
    EXPECT_THROW(argument().mandatory().flag(), argument_exception);
}

TEST_F(argument_parser_test, creating_non_bool_flag_fails)
{
    EXPECT_THROW(argument().flag().argtype(EArgType::Int), argument_exception);
    EXPECT_THROW(argument().flag().argtype(EArgType::String), argument_exception);
}

TEST_F(argument_parser_test, creating_argument_with_option_not_starting_with_dash_throws)
{
    EXPECT_THROW(argument().option("x"), argument_exception);
}

TEST_F(argument_parser_test, creating_argument_with_option_with_spaces_throws)
{
    EXPECT_THROW(argument().option("--two words"), argument_exception);
}

TEST_F(argument_parser_test, creating_argument_with_duplicate_option_throws)
{
    EXPECT_THROW(argument().option("-a").option("-a"), argument_exception);
}

TEST_F(argument_parser_test, duplicate_argument_key_throws)
{
    argument_parser uut(consts::BIN_NAME);
    uut.add_argument("file", argument().option("-f").mandatory().description("file description"));
    EXPECT_THROW(uut.add_argument("file", argument().option("-g").mandatory().description("file description")), argument_exception);
}

TEST_F(argument_parser_test, duplicate_option_throws)
{
    argument_parser uut(consts::BIN_NAME);
    uut.add_argument("file", argument().option("-f").mandatory().description("file description"));
    EXPECT_THROW(uut.add_argument("file2", argument().option("-f").mandatory().description("file description")), argument_exception);
}

TEST_F(argument_parser_test, argument_without_options_throws)
{
    argument_parser uut(consts::BIN_NAME);
    EXPECT_THROW(uut.add_argument("test", argument()), argument_exception);
}

TEST_F(argument_parser_test, parsing_arguments)
{
    argument_parser uut(consts::BIN_NAME);
    uut.add_argument("help", argument().option("-h").option("--help").flag().description("help description"));
    uut.add_argument("file", argument().option("-f").mandatory().description("file description"));
    uut.add_argument("optional", argument().option("-o").option("--optional").description("optional description"));
}

} // namespace utils
} // namespace vgraph
