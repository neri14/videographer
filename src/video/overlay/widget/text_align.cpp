#include "text_align.h"
#include "utils/logging/logger.h"

namespace vgraph {
namespace video {
namespace overlay {

ETextAlign text_align_from_string(const std::string& str)
{
    if (str == "right") {
        return ETextAlign::Right;
    } else if (str == "center") {
        return ETextAlign::Center;
    } else if (str == "left") {
        return ETextAlign::Left;
    }

    utils::logging::logger{"text_align_from_string"}
        .warning("Unknown conversion from \"{}\" to ETextAlign, falling back to ETextAlig::Left", str);
    return ETextAlign::Left;
}

PangoAlignment to_pango_align(ETextAlign align)
{
    PangoAlignment palign;
    switch (align) {
        case ETextAlign::Center:
            palign = PANGO_ALIGN_CENTER;
            break;
        case ETextAlign::Right:
            palign = PANGO_ALIGN_RIGHT;
            break;
        case ETextAlign::Left:
        default:
            palign = PANGO_ALIGN_LEFT;
            break;
    }
    return palign;
}

} // namespace overlay
} // namespace video
} // namespace vgraph
