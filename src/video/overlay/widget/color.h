#ifndef RGBA_H
#define RGBA_H

#include <string>
#include <map>

namespace vgraph {
namespace video {
namespace overlay {

struct rgba {
    double r;
    double g;
    double b;
    double a = 1.0;
};
using rgb = rgba;

namespace color {
    const rgb black   {0.00, 0.00, 0.00};
    const rgb white   {1.00, 1.00, 1.00};
    const rgb red     {1.00, 0.00, 0.00};
    const rgb lime    {0.00, 1.00, 0.00};
    const rgb blue    {0.00, 0.00, 1.00};
    const rgb yellow  {1.00, 1.00, 0.00};
    const rgb cyan    {0.00, 1.00, 1.00};
    const rgb aqua    {0.00, 1.00, 1.00};
    const rgb magenta {1.00, 0.00, 1.00};
    const rgb fuchsia {1.00, 0.00, 1.00};
    const rgb silver  {0.75, 0.75, 0.75};
    const rgb gray    {0.50, 0.50, 0.50};
    const rgb maroon  {0.50, 0.00, 0.00};
    const rgb olive   {0.50, 0.50, 0.00};
    const rgb green   {0.00, 0.50, 0.00};
    const rgb purple  {0.50, 0.00, 0.50};
    const rgb teal    {0.00, 0.50, 0.50};
    const rgb navy    {0.00, 0.00, 0.50};

    const std::map<std::string, rgba> map = {
        {"black"  , color::black},
        {"white"  , color::white},
        {"red"    , color::red},
        {"lime"   , color::lime},
        {"blue"   , color::blue},
        {"yellow" , color::yellow},
        {"cyan"   , color::cyan},
        {"aqua"   , color::aqua},
        {"magenta", color::magenta},
        {"fuchsia", color::fuchsia},
        {"silver" , color::silver},
        {"gray"   , color::gray},
        {"maroon" , color::maroon},
        {"olive"  , color::olive},
        {"green"  , color::green},
        {"purple" , color::purple},
        {"teal"   , color::teal},
        {"navy"   , color::navy}
    };
}

rgba color_from_string(const std::string& str);

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // RGBA_H
