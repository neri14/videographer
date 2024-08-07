#include <gtest/gtest.h>
#include "telemetry/fit_parser.h"

#include "testutils/testdata.h"

namespace vgraph {
namespace telemetry {
namespace consts {
    const std::string fitfile_path = std::string(TESTDATA_DIR) + "/test.fit";
    const std::string gpxfile_path = std::string(TESTDATA_DIR) + "/test.gpx";
    const std::string incorrect_path = "some/incorrect/path/to/file.fit";
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
    ASSERT_EQ(nullptr, uut->parse(consts::incorrect_path));
}

TEST_F(fit_parser_test, parse_wrong_format_returns_nullptr)
{
    ASSERT_EQ(nullptr, uut->parse(consts::gpxfile_path));
}

#if 0
//FIXME to be implemented
TEST_F(fit_parser_test, parse_fit_file)
{
    auto retval = uut->parse(consts::fitfile_path);
    ASSERT_NE(nullptr, retval);
}
#endif

} // namespace telemetry
} // namespace vgraph
