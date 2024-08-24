#ifndef TEXT_ALIGN_H
#define TEXT_ALIGN_H

#include "utils/logging/logger.h"

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

ETextAlign from_string(const std::string& str);
PangoAlignment to_pango_align(ETextAlign align);

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // TEXT_ALIGN_H
