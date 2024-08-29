#include <gtest/gtest.h>

#include "video/overlay/widget/color.h"

namespace vgraph {
namespace video {
namespace overlay {
namespace consts {
    double epsilon(0.01);
}

void expect_rgba(rgba color, std::string str)
{
    rgba result = color_from_string(str);

    EXPECT_NEAR(color.r, result.r, consts::epsilon);
    EXPECT_NEAR(color.g, result.g, consts::epsilon);
    EXPECT_NEAR(color.b, result.b, consts::epsilon);
    EXPECT_NEAR(color.a, result.a, consts::epsilon);
}

TEST(color_test, text_colors)
{
    expect_rgba({0.0, 0.0, 0.0, 1.0}, "black");
    expect_rgba({1.0, 1.0, 1.0, 1.0}, "white");
    expect_rgba({1.0, 0.0, 0.0, 1.0}, "red");
    expect_rgba({0.0, 1.0, 0.0, 1.0}, "lime");
    expect_rgba({0.0, 0.0, 1.0, 1.0}, "blue");
}

TEST(color_test, hex_colors)
{
    expect_rgba({0.0, 0.0, 0.0, 1.0}, "#000000");
    expect_rgba({1.0, 1.0, 1.0, 1.0}, "#FFFFFF");
    expect_rgba({1.0, 1.0, 1.0, 1.0}, "#ffffff");
    expect_rgba({1.0, 0.0, 0.0, 1.0}, "#FF0000");
    expect_rgba({0.0, 1.0, 0.0, 1.0}, "#00FF00");
    expect_rgba({0.0, 0.0, 1.0, 1.0}, "#0000FF");
}

TEST(color_test, hex_colors_with_alpha)
{
    expect_rgba({0.0, 0.0, 0.0, 0.0}, "#00000000");
    expect_rgba({1.0, 1.0, 1.0, 1.0}, "#FFFFFFFF");
    expect_rgba({1.0, 1.0, 1.0, 1.0}, "#ffffffff");
    expect_rgba({1.0, 0.0, 0.0, 0.5}, "#FF00007F");
    expect_rgba({0.0, 1.0, 0.0, 1.0}, "#00FF00ff");
    expect_rgba({0.0, 0.0, 1.0, 0.0}, "#0000FF00");
}

}
}
}
