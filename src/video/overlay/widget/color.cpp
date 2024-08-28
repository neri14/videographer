#include "color.h"
#include "utils/logging/logger.h"

namespace vgraph {
namespace video {
namespace overlay {

rgba color_from_string(const std::string& str)
{
    if (color::map.contains(str)) {
        return color::map.at(str);
    }

    utils::logging::logger{"text_align_from_string"}.warning("Unsupported color \"{}\" provided, falling back to white", str);
    return color::white;
}

} // namespace overlay
} // namespace video
} // namespace vgraph