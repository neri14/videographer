#include <gtest/gtest.h>
#include "telemetry/fit_file.h"

#include "testutils/testdata.h"

namespace vgraph {
namespace telemetry {
namespace consts {
    const std::string fitfile_path = std::string(TESTDATA_DIR) + "/test.fit";
    const std::string incorrect_path = "some/incorrect/path/to/file.fit";
} // namespace consts

class fit_file_test : public ::testing::Test {
protected:
    void create_uut(const std::string& path)
    {
        uut = std::make_shared<fit_file>(path);
    }

    std::shared_ptr<fit_file> uut;
};

TEST_F(fit_file_test, fit_file_object_create__ok)
{
    create_uut(consts::fitfile_path);
    ASSERT_NE(uut, nullptr);
}

TEST_F(fit_file_test, read_incorrect_path_nok)
{
    create_uut(consts::incorrect_path);
    ASSERT_EQ(uut->get_track(), nullptr);
}

#if 0
//FIXME to be implemented
TEST_F(fit_file_test, read_correct_path_ok)
{
    create_uut(consts::fitfile_path);
    ASSERT_NE(uut->get_track(), nullptr);
}
#endif

} // namespace telemetry
} // namespace vgraph
