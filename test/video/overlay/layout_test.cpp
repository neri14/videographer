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
    const std::filesystem::path correct_file = std::filesystem::path(TESTDATA_DIR) / "layout.xml";
}

TEST(layout_test, load_nonexistant_file_fails)
{
    EXPECT_EQ(nullptr, load_layout(consts::nonexistant_file));
}

TEST(layout_test, load_empty_xml_fails)
{
    EXPECT_EQ(nullptr, load_layout(consts::empty_file));
}

TEST(layout_test, load_different_xml_fails)
{
    EXPECT_EQ(nullptr, load_layout(consts::different_file));
}

TEST(layout_test, load_correct_xml_file)
{
    EXPECT_NE(nullptr, load_layout(consts::correct_file));
}

}
}
}
