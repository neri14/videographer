#ifndef TEXT_ALIGN_H
#define TEXT_ALIGN_H

#include <map>
#include <string>

extern "C" {
    #include <pango/pango.h>
}

namespace vgraph {
namespace video {
namespace overlay {

enum class ETextAlign {
    Left,
    Center,
    Right
};

ETextAlign text_align_from_string(const std::string& str);
PangoAlignment to_pango_align(ETextAlign align);

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // TEXT_ALIGN_H
