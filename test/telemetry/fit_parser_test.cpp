#include "telemetry/fit_parser.h"

#include "testutils/testdata.h"

#include <gtest/gtest.h>
#include <filesystem>

namespace vgraph {
namespace telemetry {
namespace consts {
    const std::filesystem::path fit_path = std::filesystem::path(TESTDATA_DIR) / "correct.fit";
    const std::filesystem::path gpx_path = std::filesystem::path(TESTDATA_DIR) / "correct.gpx";
    const std::filesystem::path broken_fit_path = std::filesystem::path(TESTDATA_DIR) / "broken.fit";
    const std::filesystem::path nonexistant_path = std::filesystem::path("some/incorrect/path/to/file.fit");
} // namespace consts

class fit_parser_test : public ::testing::Test {
protected:
    void SetUp() override
    {
        uut = std::make_shared<fit_parser>();
    }

    void TearDown() override
    {
        uut.reset();
    }

    std::shared_ptr<fit_parser> uut;
};

TEST_F(fit_parser_test, parse_incorrect_path_returns_nullptr)
{
    ASSERT_EQ(nullptr, uut->parse(consts::nonexistant_path));
}

TEST_F(fit_parser_test, parse_wrong_file_extension_returns_nullptr)
{
    ASSERT_EQ(nullptr, uut->parse(consts::gpx_path));
}

TEST_F(fit_parser_test, parse_broken_file_returns_nullptr)
{
    auto retval = uut->parse(consts::broken_fit_path);
    ASSERT_EQ(nullptr, retval);
}

TEST_F(fit_parser_test, parse_correct_file_returns_sequence)
{
    auto retval = uut->parse(consts::fit_path);
    ASSERT_NE(nullptr, retval);
}

} // namespace telemetry
} // namespace vgraph
