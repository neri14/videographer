#include <gtest/gtest.h>
#include <cstring>

#include "utils/argument_parser.h"

namespace vgraph {
namespace utils {

class argument_parser_test: public ::testing::Test{
protected:
    void TearDown() override
    {
        destroy_args();
    }

    void create_args(std::vector<std::string> vargs)
    {
        argc = vargs.size() + 1;
        argv = new char*[argc];

        argv[0] = new char[6];
        argv[0] = std::strcpy(argv[0], "dummy");

        int i = 1;
        for (const std::string& arg : vargs) {
            argv[i] = new char[arg.size() + 1];
            std::strcpy(argv[i], arg.c_str());
            i++;
        }
    }

    void destroy_args()
    {
        if (argv != nullptr) {
            for (int i = 0; i < argc; i++) {
                delete[] argv[i];
            }
            delete[] argv;

            argv = nullptr;
            argc = 0;
        }
    }

    int argc = 0;
    char** argv = nullptr;
};


/* ARGUMENT CREATION */
TEST_F(argument_parser_test, creating_mandatory_flag_throws)
{
    EXPECT_THROW(argument().flag().mandatory(), argument_exception);
    EXPECT_THROW(argument().mandatory().flag(), argument_exception);
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

/* ADDING ARGUMENT */
TEST_F(argument_parser_test, duplicate_argument_key_throws)
{
    argument_parser uut("binary");
    uut.add_argument("file", argument().option("-f").mandatory().description("file description"));
    EXPECT_THROW(uut.add_argument("file", argument().option("-g").mandatory().description("file description")), argument_exception);
}

TEST_F(argument_parser_test, duplicate_option_throws)
{
    argument_parser uut("binary");
    uut.add_argument("file", argument().option("-f").mandatory().description("file description"));
    EXPECT_THROW(uut.add_argument("file2", argument().option("-f").mandatory().description("file description")), argument_exception);
}

TEST_F(argument_parser_test, argument_without_options_throws)
{
    argument_parser uut("binary");
    EXPECT_THROW(uut.add_argument("test", argument()), argument_exception);
}

TEST_F(argument_parser_test, parsing_arguments)
{
    argument_parser uut("binary");
    uut.add_argument("help", argument().option("-h").option("--help").flag().description("help description"));
    uut.add_argument("file", argument().option("-f").mandatory().description("file description"));
    uut.add_argument("optional", argument().option("-o").option("--optional").description("optional description"));
}

//TODO nok tests for getters without parsing implementation

/* PARSING AND GETTING VALUES */

// TEST_F(argument_parser_test, string_value_parsing)
// {}

TEST_F(argument_parser_test, not_parsed_argument_value_is_not_present)
{
    argument_parser uut("binary");
    uut.add_argument("file", argument().option("-f").mandatory().description("file description"));

    create_args({});
    uut.parse(argc, argv);

    EXPECT_FALSE(uut.has("file"));
    EXPECT_THROW(uut.get<std::string>("file"), argument_exception);
}

// TEST_F(argument_parser_test, parsed_flag_is_true)
// {}

// TEST_F(argument_parser_test, parsing_undefined_arguments_throws)
// {}

TEST_F(argument_parser_test, not_prased_flag_is_false)
{
    argument_parser uut("binary");
    uut.add_argument("help", argument().option("-h").option("--help").flag().description("help description"));

    create_args({});
    uut.parse(argc, argv);

    EXPECT_FALSE(uut.has("help"));
    EXPECT_FALSE(uut.get<bool>("help"));
}

TEST_F(argument_parser_test, undefined_argument_retrieval_throws)
{
    argument_parser uut("binary");

    create_args({});
    uut.parse(argc, argv);

    EXPECT_THROW(uut.has("arg"), argument_exception);
    EXPECT_THROW(uut.get<std::string>("arg"), argument_exception);
    EXPECT_THROW(uut.get<bool>("arg"), argument_exception);
}

// TEST_F(argument_parser_test, flag_string_value_retrieval_throws)
// {}

// TEST_F(argument_parser_test, unsupported_type_retrieval_throws)
// {}

} // namespace utils
} // namespace vgraph
