#include <gtest/gtest.h>

#include <filesystem>
#include "video/overlay/layout_parser.h"

namespace vgraph {
namespace video {
namespace overlay {
namespace consts {
    const std::filesystem::path nonexistant_file = std::filesystem::path(TESTDATA_DIR) / "nosuchfile.xml";
    const std::filesystem::path empty_file = std::filesystem::path(TESTDATA_DIR) / "empty.xml";
    const std::filesystem::path different_file = std::filesystem::path(TESTDATA_DIR) / "different.xml";
    const std::filesystem::path correct_file = std::filesystem::path(TESTDATA_DIR) / "layout.xml";
}

class layout_parser_test : public ::testing::Test {
protected:
    void SetUp() override
    {
        uut = std::make_shared<layout_parser>();
    }

    void TearDown() override
    {
        uut.reset();
    }

    std::shared_ptr<layout_parser> uut;
};

TEST_F(layout_parser_test, load_nonexistant_file_fails)
{
    std::shared_ptr<layout> res = uut->parse(consts::nonexistant_file);
    EXPECT_EQ(nullptr, res);
}

TEST_F(layout_parser_test, load_empty_xml_fails)
{
    std::shared_ptr<layout> res = uut->parse(consts::empty_file);
    EXPECT_EQ(nullptr, res);
}

TEST_F(layout_parser_test, load_different_xml_fails)
{
    std::shared_ptr<layout> res = uut->parse(consts::different_file);
    EXPECT_EQ(nullptr, res);
}

TEST_F(layout_parser_test, load_correct_xml_file)
{
    std::shared_ptr<layout> res = uut->parse(consts::correct_file);
    EXPECT_NE(nullptr, res);
    EXPECT_GT(res->size(), 0);
}

}
}
}
