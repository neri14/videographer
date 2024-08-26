#include <gtest/gtest.h>

#include <filesystem>
#include "video/overlay/layout.h"

namespace vgraph {
namespace video {
namespace overlay {
namespace consts {
    const std::filesystem::path nonexistant_file = std::filesystem::path(TESTDATA_DIR) / "nosuchfile.xml";
    const std::filesystem::path empty_file = std::filesystem::path(TESTDATA_DIR) / "empty.xml";
    const std::filesystem::path different_file = std::filesystem::path(TESTDATA_DIR) / "different.xml";
    const std::filesystem::path correct_file = std::filesystem::path(TESTDATA_DIR) / "template.xml";
}

TEST(layout_test, load_nonexistant_file_fails)
{
    EXPECT_EQ(nullptr, layout::load(consts::nonexistant_file));
}

TEST(layout_test, load_empty_xml_fails)
{
    EXPECT_EQ(nullptr, layout::load(consts::empty_file));
}

TEST(layout_test, load_different_xml_fails)
{
    EXPECT_EQ(nullptr, layout::load(consts::different_file));
}

// TEST(layout_test, load_correct_xml_file)
// {
//     EXPECT_NE(nullptr, layout::load(consts::correct_file));
// }

}
}
}
