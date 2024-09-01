#include <gtest/gtest.h>
#include "arguments.h"
#include "testutils/testdata.h"

#include <filesystem>

namespace vgraph {
namespace consts {
    const std::filesystem::path full_conf = std::filesystem::path(TESTDATA_DIR) / "full.conf";
    const std::filesystem::path partial_conf = std::filesystem::path(TESTDATA_DIR) / "partial.conf";
}

//forward declaration because function under test is internal to cpp
bool parse_config_file(const std::string& path, arguments& args);

TEST(config_file_parsing_test, full_config_file)
{
    arguments args;

    EXPECT_TRUE(parse_config_file(consts::full_conf, args));

    EXPECT_TRUE(args.debug);
    EXPECT_TRUE(args.gpu);
    EXPECT_TRUE(args.timecode);
    EXPECT_TRUE(args.alignment_mode);
    EXPECT_EQ("/some/absolute/path", args.telemetry);
    EXPECT_EQ("./a/different/relative/path", args.layout);
    EXPECT_EQ("mp4_file.mp4", args.input);
    EXPECT_EQ("/absolute/mp4/file.mp4", args.output);
    EXPECT_EQ(100000, args.bitrate);

    ASSERT_TRUE(args.resolution);
    EXPECT_EQ(1234, args.resolution->first);
    EXPECT_EQ(567, args.resolution->second);

    ASSERT_TRUE(args.offset);
    EXPECT_NEAR(123.456, *args.offset, 0.001);

    ASSERT_TRUE(args.clip_time);
    EXPECT_NEAR(60, *args.clip_time, 0.001);
}

TEST(config_file_parsing_test, partial_config_file)
{
    arguments args;

    EXPECT_TRUE(parse_config_file(consts::partial_conf, args));

    EXPECT_FALSE(args.debug);
    EXPECT_TRUE(args.gpu);
    EXPECT_FALSE(args.timecode);
    EXPECT_FALSE(args.alignment_mode);
    EXPECT_EQ("./test/testdata/correct.fit", args.telemetry);
    EXPECT_EQ("./test/testdata/layout.xml", args.layout);
    EXPECT_FALSE(args.input);
    EXPECT_FALSE(args.output);
    EXPECT_EQ(80000, args.bitrate);
    EXPECT_FALSE(args.offset);
    EXPECT_FALSE(args.clip_time);

    ASSERT_TRUE(args.resolution);
    EXPECT_EQ(3840, args.resolution->first);
    EXPECT_EQ(2160, args.resolution->second);
}

} // namespace vgraph
