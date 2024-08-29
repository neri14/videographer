#include "color.h"
#include "utils/logging/logger.h"

#include <sstream>
#include <iostream>

namespace vgraph {
namespace video {
namespace overlay {

rgba color_from_string(const std::string& str)
{
    if (color::map.contains(str)) {
        return color::map.at(str);
    } else if (str.starts_with('#') && str.size() >= 7 && str.size() <= 9) {
        rgba res {
            std::stoi(str.substr(1, 2), nullptr, 16) / 255.0,
            std::stoi(str.substr(3, 2), nullptr, 16) / 255.0,
            std::stoi(str.substr(5, 2), nullptr, 16) / 255.0
        };

        if (str.size() == 9) {
            res.a = std::stoi(str.substr(7, 2), nullptr, 16) / 255.0;
        }

        return res;
    }

    utils::logging::logger{"color_from_string"}.warning("Failed to parse provided color \"{}\", falling back to white", str);
    return color::white;
}

} // namespace overlay
} // namespace video
} // namespace vgraph